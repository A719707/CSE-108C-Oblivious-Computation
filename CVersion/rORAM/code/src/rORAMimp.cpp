#include <iostream>
#include <vector>
#include <bitset>
#include <random>
#include <algorithm>
#include <cmath>
#include <cassert>
#include <cstring>
#include <thread>
#include <chrono>
#include <map>
#include <set>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <fstream>
#include <sstream>

using namespace std;

class Utils {
public:
    static int iBitsPrefix(int n, int w, int i) {
        return (~((1 << (w - i)) - 1)) & n;
    }

    static void refineKeys(vector<int>& keys, int d, int z) {
        int j = 0, k = 1;
        for (int i = 1; i <= d && j < keys.size(); i++) {
            while (j < keys.size() && keys[j] == i) {
                keys[j] = (i - 1) * z + k;
                j++;
                k++;
            }
            if (k <= z) 
                k = 1;
            else
                k -= z;
        }
    }

    static void writePositionMap(vector<bitset<1024>>& map, int index, int val, int d) {
        int base = (index % 4) * d;
        writeBitSet(map[index / 4], base, val, d);
    }

    static void writeBitSet(bitset<1024>& map, int base, int val, int d) {
        for (int i = 0; i < d; i++) {
            if (((val >> i) & 1) == 1)
                map.set(base + i);
            else
                map.reset(base + i);
        }
    }

    static int readPositionMap(vector<bitset<1024>>& map, int index, int d) {
        int base = fastMod(index, 4) * d;
        int mapIdx = fastDivide(index, 4);
        if (mapIdx >= map.size()) {
            cerr << "Coding FAIL!" << endl;
            exit(1);
        }
        return readBitSet(map[mapIdx], base, d);
    }

    static int readBitSet(bitset<1024>& map, int base, int d) {
        int ret = 0;
        for (int i = 0; i < d; i++) {
            if (map.test(base + i))
                ret ^= (1 << i);
        }
        return ret;
    }

    static int bitLength(int n) {
        if (n == 0) 
            return 1;

        int res = 0;
        do {
            n = n >> 1;
            res++;
        } while (n > 0);
        return res;
    }

    static int fastMod(int a, int b) {
        // b is a power of 2
        int shifts = (int)(log(b) / log(2));
        return a & ((1 << shifts) - 1);
    }

    static int fastDivide(int a, int b) {
        // b is a power of 2
        int shifts = (int)(log(b) / log(2));
        return a >> shifts;
    }

    static vector<uint8_t> genPRBits(mt19937& rnd, int len) {
        vector<uint8_t> b(len);
        for (int i = 0; i < len; ++i) {
            b[i] = rnd();
        }
        return b;
    }
};

class PathORAMBasic {
public:
    class Tree {
    public:
        class Block {
        public:
            bitset<1024> data;
            int id;
            int treeLabel;
            vector<uint8_t> r;

            Block() : id(0), treeLabel(0) {}

            Block(const bitset<1024>& data, int id, int label) : data(data), id(id), treeLabel(label) {}

            Block(const vector<uint8_t>& bytes) {
                ByteBuffer bb(bytes);
                vector<uint8_t> bs(dataSize);
                bb.get(bs);
                data = bitset<1024>(bs);
                id = bb.getInt();
                treeLabel = bb.getInt();
                r.resize(nonceLen);
                bb.get(r);
            }

            Block(const vector<uint8_t>& bytes, bool stash) {
                ByteBuffer bb(bytes);
                vector<uint8_t> bs(dataSize);
                bb.get(bs);
                data = bitset<1024>(bs);
                id = bb.getInt();
                treeLabel = bb.getInt();
            }

            bool isDummy() const {
                assert(r.empty());
                return id == N;
            }

            void erase() {
                id = N;
                treeLabel = 0;
            }

            vector<uint8_t> toByteArray() const {
                ByteBuffer bb(extDataSize + nonceLen);
                vector<uint8_t> d(dataSize);
                vector<uint8_t> temp = data.toByteArray();
                copy(temp.begin(), temp.end(), d.begin());
                bb.put(d);
                bb.putInt(id);
                bb.putInt(treeLabel);
                bb.put(r.empty() ? vector<uint8_t>(nonceLen) : r);
                return bb.array();
            }

            string toString() const {
                return toByteArray().toString();
            }

            void enc() {
                r = Utils::genPRBits(rnd, nonceLen);
                mask();
            }

            void dec() {
                mask();
                r.clear();
            }

            void mask() {
                vector<uint8_t> mask(extDataSize);
                try {
                    MessageDigest sha1 = MessageDigest.getInstance("SHA-1");
                    int hashLength = 20;
                    int i = 0;
                    for (; (i + 1) * hashLength < extDataSize; i++) {
                        sha1.update(clientKey);
                        sha1.update(r);
                        sha1.update(ByteBuffer::allocate(4).putInt(i));
                        copy(sha1.digest().begin(), sha1.digest().end(), mask.begin() + i * hashLength);
                    }
                    sha1.update(clientKey);
                    sha1.update(r);
                    sha1.update(ByteBuffer::allocate(4).putInt(i));
                    copy(sha1.digest().begin(), sha1.digest().end(), mask.begin() + i * hashLength, mask.begin() + extDataSize - i * hashLength);

                    bitset<1024> dataMask = bitset<1024>(vector<uint8_t>(mask.begin(), mask.begin() + dataSize));
                    data ^= dataMask;
                    id ^= ByteBuffer::wrap(mask, dataSize, 4).getInt();
                    treeLabel ^= ByteBuffer::wrap(mask, dataSize + 4, 4).getInt();
                } catch (const std::exception& e) {
                    cerr << e.what() << endl;
                    exit(1);
                }
            }
        };

        class Bucket {
        public:
            vector<Block> blocks;

            Bucket(const Block& b) {
                blocks.push_back(b);
                for (int i = 1; i < Z; i++)
                    blocks.push_back(Block());
            }

            Bucket(const vector<uint8_t>& array) {
                ByteBuffer bb(array);
                vector<uint8_t> temp(extDataSize + nonceLen);
                for (int i = 0; i < Z; i++) {
                    bb.get(temp);
                    blocks.push_back(Block(temp));
                }
            }

            vector<uint8_t> toByteArray() const {
                ByteBuffer bb(Z * (extDataSize + nonceLen));
                for (const Block& blk : blocks)
                    bb.put(blk.toByteArray());
                return bb.array();
            }

            void encryptBlocks() {
                for (Block& blk : blocks)
                    blk.enc();
            }
        };

        class Stash {
        public:
            vector<Block> blocks;
            int size;
            int recLevel;

            Stash(int size, int recLevel, bool useLS) : size(size), recLevel(recLevel) {
                if (useLS) {
                    // Implement LocalStorage logic here
                } else {
                    blocks.resize(size);
                }
            }

            void save(ObjectOutputStream& os) const {
                os.writeInt(size);
                os.writeInt(recLevel);
                bool useLS = false;
                os.writeBoolean(useLS);
                if (useLS) {
                    // Implement LocalStorage logic here
                } else {
                    os.writeInt(blocks.size());
                    for (const Block& blk : blocks) {
                        vector<uint8_t> data = blk.toByteArray();
                        os.writeInt(data.size());
                        os.write(data.data(), data.size());
                    }
                }
            }

            Stash(ObjectInputStream& is) {
                size = is.readInt();
                recLevel = is.readInt();
                bool useLS = is.readBoolean();
                if (useLS) {
                    // Implement LocalStorage logic here
                } else {
                    blocks.resize(size);
                    int s = is.readInt();
                    for (int i = 0; i < s; i++) {
                        int byteSize = is.readInt();
                        vector<uint8_t> data(byteSize);
                        is.readFully(data.data(), byteSize);
                        blocks[i] = Block(data, true);
                    }
                }
            }
        };

        int N;
        int D;
        ExternalStorageInterface storedTree;
        long treeSize;
        Stash stash;
        int oramID;

        Tree() {}

        vector<bitset<1024>> initialize(ExternalStorageInterface si, int maxBlocks, int dSize, vector<bitset<1024>>& data, int oramID) {
            storedTree = si;
            dataSize = dSize;
            extDataSize = dataSize + 4 + 4;

            vector<int> permutation(maxBlocks);
            iota(permutation.begin(), permutation.end(), 0);
            shuffle(permutation.begin(), permutation.end(), rnd);

            this->oramID = oramID;
            buildTree(maxBlocks, permutation, data);
            stash = Stash(stashSize, recLevel, stashUseLS);

            vector<bitset<1024>> posMap((N + C - 1) / C);
            for (int i = 0; i < posMap.size(); i++) {
                posMap[i] = bitset<1024>();
            }

            for (int i = 0; i < N; i++) {
                int p = i;
                if (i < permutation.size()) {
                    p = permutation[i];
                }
                Utils::writePositionMap(posMap, i, p, D);
            }

            return posMap;
        }

        void buildTree(int maxBlocks, const vector<int>& permutation, vector<bitset<1024>>& dataArray) {
            SessionState ss = SessionState::getInstance();
            map<string, Request> fastInitMap = ss.fastInitMap;
            if (!ss.fastInit) {
                fastInitMap.clear();
            }

            N = (int)pow(2, ceil(log(maxBlocks) / log(2)));
            D = Utils::bitLength(N) - 1;

            const int removeIntervalSize = 512;
            const double sizeFactorForSlowdown = 0.75;
            const int logIntervalSize = 512;
            vector<Pollable> v;

            treeSize = 2 * N - 1;
            for (int i = 0; i < treeSize; i++) {
                Bucket temp;
                if (i < treeSize / 2) {
                    temp = Bucket(Block());
                } else {
                    if (i - N + 1 < maxBlocks) {
                        int id = find(permutation.begin(), permutation.end(), i - N + 1) - permutation.begin();
                        int label = i - N + 1;

                        bitset<1024> data;
                        string blockIdStr = to_string(id);
                        if (recLevel == 0 && fastInitMap.find(blockIdStr) != fastInitMap.end()) {
                            Request req = fastInitMap[blockIdStr];
                            if (req.getType() != RequestType::PUT) {
                                cerr << "Error: Invalid request type." << endl;
                                exit(1);
                            }
                            PutRequest put = static_cast<PutRequest&>(req);
                            vector<uint8_t> val = put.getValue().getData();
                            if (val.size() > dataSize) {
                                cerr << "Error: Value size exceeds data size." << endl;
                                exit(1);
                            }
                            data = bitset<1024>(val);
                        } else {
                            if (!dataArray.empty()) {
                                if (dataArray.size() <= id) {
                                    cerr << "Error: Invalid data array index." << endl;
                                    exit(1);
                                }
                                data = dataArray[id];
                            } else {
                                vector<long> val(1, id);
                                data = bitset<1024>(val);
                            }
                        }

                        temp = Bucket(Block(data, id, label));
                    } else {
                        temp = Bucket(Block());
                    }
                }

                temp.encryptBlocks();

                DataItem di = SimpleDataItem(temp.toByteArray());
                int fileLabel = i / bucketLimitPerFile;
                string objectKey = to_string(oramID) + to_string(recLevel) + to_string(fileLabel);
                int fileOffset = i % bucketLimitPerFile;

                UploadObject upload(Request::initReqId, objectKey, di);
                upload.setObjectOffset(fileOffset * Z * (extDataSize + nonceLen));
                upload.setObjectSize(Z * (extDataSize + nonceLen));
                ScheduledOperation sop = storedTree.uploadObject(upload);

                v.push_back(sop);

                if (i > 0 && (i % removeIntervalSize) == 0) {
                    Pollable::removeCompleted(v);

                    if (v.size() >= (int)(removeIntervalSize * sizeFactorForSlowdown)) {
                        cout << "Slowing down so storage can catch up..." << endl;
                        int factor = (int)ceil(v.size() / removeIntervalSize);
                        if (factor > 5) {
                            factor = 5;
                        }
                        this_thread::sleep_for(chrono::milliseconds(factor * 5));
                        Pollable::removeCompleted(v);
                    }
                }

                if (i > 0 && (i % logIntervalSize) == 0) {
                    cout << "Uploaded " << (i - v.size()) << " nodes so far." << endl;
                }
            }

            Pollable::waitForCompletion(v);
        }

        vector<Block> readBuckets(long reqId, int leafLabel) {
            vector<Bucket> buckets = getBucketsFromPath(reqId, leafLabel);
            vector<Block> res(Z * buckets.size());
            int i = 0;
            for (const Bucket& bkt : buckets) {
                for (const Block& blk : bkt.blocks) {
                    res[i++] = Block(blk);
                }
            }
            return res;
        }

        vector<Bucket> getBucketsFromPath(long reqId, int leaf) {
            vector<Bucket> ret(D + 1);
            vector<ScheduledOperation> v;

            int temp = leaf;
            for (int i = 0; i < ret.size(); i++) {
                int fileLabel = temp / bucketLimitPerFile;
                string objectKey = to_string(oramID) + to_string(recLevel) + to_string(fileLabel);
                int fileOffset = temp % bucketLimitPerFile;

                DownloadObject download(reqId, objectKey);
                download.setObjectOffset(fileOffset * Z * (extDataSize + nonceLen));
                download.setObjectSize(Z * (extDataSize + nonceLen));
                ScheduledOperation sop = storedTree.downloadObject(download);
                v.push_back(sop);

                if (temp > 0) {
                    temp = ((temp + 1) >> 1) - 1;
                }
            }

            Pollable::waitForCompletion(v);
            for (int i = 0; i < ret.size(); i++) {
                ret[i] = Bucket(v[i].getDataItem().getData());
            }

            return ret;
        }

        void save(ObjectOutputStream& os) const {
            os.writeInt(N);
            os.writeInt(D);
            os.writeLong(treeSize);
            stash.save(os);
        }

        Tree(ExternalStorageInterface si, ObjectInputStream& is) {
            storedTree = si;
            N = is.readInt();
            D = is.readInt();
            treeSize = is.readLong();
            stash = Stash(is);
        }
    };

    PathORAMBasic(mt19937& rand) : rnd(rand) {
        clientKey = Utils::genPRBits(rnd, keyLen);
    }

    vector<bitset<1024>> initialize(ExternalStorageInterface si, int maxBlocks, int unitB, vector<bitset<1024>>& data, int recLevel, int oramID) {
        assert(maxBlocks < (~(1LL << 63))) : "Too many blocks in a tree.";
        this->recLevel = recLevel;
        int nextB = unitB;
        serverTree = Tree();
        vector<bitset<1024>> posMap = serverTree.initialize(si, maxBlocks, nextB, data, oramID);
        return posMap;
    }

    enum class OpType { Read, Write };

    Tree::Block access(long reqId, vector<bitset<1024>>& posMap, OpType op, int a, bitset<1024> data) {
        Tree& tr = serverTree;

        int leafLabel = tr.N - 1 + Utils::readPositionMap(posMap, a, tr.D);
        vector<Tree::Block> blocks = tr.readBuckets(reqId, leafLabel);

        int newLabel = rnd() % tr.N;
        Utils::writePositionMap(posMap, a, newLabel, tr.D);

        return rearrangeBlocksAndReturn(reqId, op, a, data, leafLabel, blocks, newLabel);
    }

    Tree::Block rearrangeBlocksAndReturn(long reqID, OpType op, int a, bitset<1024> data, int leafLabel, vector<Tree::Block>& blocks, int newlabel) {
        Tree& tr = serverTree;
        Tree::Block res;
        vector<Tree::Block> unionBlocks(tr.stash.size + blocks.size());

        for (int i = 0; i < tr.stash.size; i++) {
            Tree::Block blk;
            if (i < tr.stash.blocks.size()) {
                blk = tr.stash.blocks[i];
            } else {
                blk = Tree::Block();
            }
            unionBlocks[i] = blk;
        }

        for (int i = 0; i < blocks.size(); i++) {
            unionBlocks[i + tr.stash.size] = Tree::Block(blocks[i]);
        }

        for (Tree::Block& blk : unionBlocks) {
            if (!blk.r.empty()) {
                blk.dec();
            }
        }

        for (int i = 0; i < unionBlocks.size(); i++) {
            Tree::Block& blk = unionBlocks[i];
            if (blk.id == a) {
                res

from cryptography.fernet import Fernet

message = 1
message1 = 1

key = Fernet.generate_key()
fernet = Fernet(key)
encMessage = fernet.encrypt(message.encode())
encMessage1 = fernet.encrypt(message1.encode())

print("original string: ", message)
print("encrypted string: ", encMessage)
print("encrypted string: ", encMessage1)

decMessage = fernet.decrypt(encMessage).decode()
decMessage1 = fernet.decrypt(encMessage1).decode()

print("decrypted string: ", decMessage)
print("decrypted string: ", decMessage1)

print(decMessage1 == decMessage)
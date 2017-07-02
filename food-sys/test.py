import pexpect


cons_cook_list =[]
cons_client_list=[]
for i in range(1050):
    cons_cook = pexpect.spawn("./cook-client 127.0.0.1")
    cons_cook.sendline("1");
    cons_client = pexpect.spawn("./client 127.0.0.1")
    cons_client.sendline("1");
    cons_cook_list.append(cons_cook)
    cons_client_list.append(cons_client)
    print("num of clients ",(i+1)*2)

input("Press enter and exit")

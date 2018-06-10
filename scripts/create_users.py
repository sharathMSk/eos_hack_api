import os
import sys
import subprocess
import time
import json
number_of_users = int(sys.argv[2])


#start cleos and setup wallet.

users_dict = {}
user_prefix = sys.argv[1]
print "Creating wallet for LoanBlock users"
s = subprocess.check_output("docker exec eosio /opt/eosio/bin/cleos --wallet-url http://localhost:8888 wallet create -n users_wallet | awk 'FNR == 4'", shell=True)
s = s.replace("\"","")
wallet_password = s
print ".....Done....."


print "Creating user account and keys"
for i in range(1, number_of_users+1):
    user = {}
    print "Creating account for user {0}{1}".format(user_prefix,i)
    print "----> creating keys"
    s = subprocess.check_output("docker exec eosio /opt/eosio/bin/cleos --wallet-url http://localhost:8888 create key",shell=True)
    #print s
    s = s.split("\n")
    key_1 = s[0].split(" ")
    key_2 = s[1].split(" ")
    user[key_1[0]] = key_1[2]
    user[key_2[0]] = key_2[2]

    #print "\n\n Pvt->{0} Pub->{1}\n\n".format(key_1,key_2)
    print "----> importing private key into wallet"
    s = subprocess.check_output("docker exec eosio /opt/eosio/bin/cleos --wallet-url http://localhost:8888 wallet import -n users_wallet {0}".format(key_1[2]), shell=True)
    #print s

    print "----> creating the eosio account"
    s = subprocess.check_output("docker exec eosio /opt/eosio/bin/cleos --wallet-url http://localhost:8888 create account eosio {0}{1} {2} {2}".format(user_prefix,i,key_2[2]), shell=True)
    #print s
    users_dict["{0}{1}".format(user_prefix,i)] = user
    print "----> Done"
    user["account"] = "{0}{1}".format(user_prefix,i)
with open('dump/users_account.json','w') as file:
    file.write(json.dumps(users_dict))

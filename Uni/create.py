import json
import time

import firebase_admin
from firebase_admin import db

cred_obj = firebase_admin.credentials.Certificate(add your working path)
default_app = firebase_admin.initialize_app(cred_obj, {
   add your database url path
})


class buildDatabase:
    # initial
    def __init__(self) -> None:
        self.db = {
            "usr_count": 0
        }
        self.refresh_before_modify = False # change this to true if you have multiple access to the server
                                           # this will will tell the script to receive the data
                                           # again for integrity but it will add overhead in
                                           # your software. You can also use hashing AES or SHA
                                           # to detect changes in the data
        self.pullAll_fromServer()
        self.ref = db.reference("/")
    # pull data from server
    def pullAll_fromServer(self) -> None:
        self.ref = db.reference("/")
        self.db = self.ref.get()
        if (not self.db):
            self.db = {
                "usr_count": 0
            }
            self.pushAll_toServer()
    # push data 
    def pushAll_toServer(self):
        self.ref = db.reference("/")
        self.ref.set(self.db)
    # add user info
    def add_account(self, name, address= None, phone = None) -> None:
        for info in self.db.values():
            if not type(info) is int:
                if info['name'] == name:
                    print("account already exist")
                    return None
        self.db['usr_count'] += 1
        self.ref = db.reference("/usr_count")
        self.ref.set(self.db['usr_count'])
        self.ref = db.reference("/")
        self.ref.push().set(
            {
                'name': name,
                'address': address if address else '<not set>',
                'phone': phone if phone else '<not set>'
            }
        )
    # update info
    def update(self, name, address = None, phone = None) -> None:
        self.ref = db.reference("/")
        self.pullAll_fromServer()
        for key, value in self.db.items():
            if (value["name"] == name):
                if address:
                    value['address'] = address
                if phone:
                    value['phone'] = phone
                self.ref.child(key).update(value)
    # delete info
    def rem_account(self, name) -> None:
        if self.refresh_before_modify: # use this if you have multiple access to the server
            self.pullAll_fromServer()
        self.ref = db.reference('/')
        deleted = None
        for key, value in self.db.items():
            if key != 'usr_count':
                if (value["name"] == name):
                    print("account found and proceed delete\n")
                    self.db['usr_count'] -= 1
                    self.ref.child('usr_count').set(
                        self.db['usr_count']
                    )
                    deleted = key
                    self.ref.child(key).set({})
                    break
        if (deleted):
            del  self.db[deleted]
        else:
            print("account not found please try another account\n")

    def remove_account_baseOn_ID(self, ID):
        if self.refresh_before_modify:
            self.pullAll_fromServer()
        self.ref = db.reference('/')
        deleted = None
        for key, value in self.db.items():
            if (key == ID):
                print("account found and proceed delete\n")
                self.db['usr_count'] -= 1
                self.ref.child('usr_count').set(
                    self.db['usr_count']
                ) # also update the server but the count value
                deleted = key # this added to remove from the local dictionary
                              # delete the local value is faster due to no need to receive
                              # datas from live server, but use this only there is no
                              # second access to the database, because the local data may mismatch
                              # is second or more access remove the data, and that wont reflect the
                              # data on the server, set the `self.refresh_before_modify` to true to
                              # to support multiple access.
                self.ref.child(key).set({}) # update the server
                break
        if (deleted):
            del  self.db[deleted]

    # add content to info
    def get_account_content(self, name) -> dict:
        self.pullAll_fromServer()
        if (not self.db['usr_count']):
            return {}
        for key, value in self.db.items():
            if (value["name"] == name):
                return value
        return {}
    # add info base on ID
    def get_account_baseOn_ID(self, ID) -> dict:
        self.pullAll_fromServer()
        if not self.db['usr_count']:
            return {}
        if ID in self.db:
            return self.db[ID]
        return {}
    

print('run code successfully')
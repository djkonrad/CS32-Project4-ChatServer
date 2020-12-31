//
//  ChatTracker.cpp
//  ChatTracker
//
//  Created by Dom's Mac on 5/28/20.
//  Copyright © 2020 Dom's Mac. All rights reserved.
//

#include "ChatTracker.h"
#include <string>
#include <vector>
#include <list>
#include <functional>
using namespace std;

struct Info
{
    Info(string u, string c, int count = 0) : userName(u), chatName(c), m_count(count) {}
    string userName;
    string chatName;
    int m_count;
};

class userHashTable
{
public:
    userHashTable(int n);
    ~userHashTable();
    void insert(Info u);
    int searchAndDestroy(string user, string chat);
    unsigned int hashFunc(string s);
    list<Info>* getTable();
    int getSize();
    
private:
    list<Info>* m_table;
    int m_size;
};

userHashTable::userHashTable(int n)
{
    m_table = new list<Info>[n];
    m_size = n;
}

userHashTable:: ~userHashTable()
{
    delete[] m_table;
}

list<Info>* userHashTable::getTable()
{
    return m_table;
}

int userHashTable:: getSize()
{
    return m_size;
}

unsigned int userHashTable:: hashFunc(string s)
{
    hash<string> str_hash;
    unsigned int hashValue = str_hash(s);
    unsigned int bucketNum = hashValue % m_size;
    return bucketNum;
}

void userHashTable::insert(Info u)
{
    int index = hashFunc(u.userName);
    m_table[index].push_front(u);
}

//finds the Info struct  with same user and chat names passed as arguments
//deletes the struct  if is found and returns the amount of times the user has contributed to the chat
//if the struct pointer is not found, the function returns 0
int userHashTable::searchAndDestroy(string user, string chat)
{
    int timesContributedToChat = 0;
    int index = hashFunc(user);
    list<Info>::iterator it = m_table[index].begin();
    while(it != m_table[index].end())
    {
        if((it)->userName == user && (it)->chatName == chat)
        {
            timesContributedToChat = (it)->m_count;
            m_table[index].erase(it);
            return timesContributedToChat;
        }
        it++;
    }
    return timesContributedToChat;
}

class ChatTrackerImpl
{
  public:
    ChatTrackerImpl(int maxBuckets);
    void join(string user, string chat);
    int terminate(string chat);
    int contribute(string user);
    int leave(string user, string chat);
    int leave(string user);

  private:
    userHashTable m_user;
    vector<Info>m_usersWhoLeft;
};

ChatTrackerImpl::ChatTrackerImpl(int maxBuckets): m_user(maxBuckets)
{}

void ChatTrackerImpl::join(string user, string chat)
{
        //allocate memory for user that is going to join chat
    
        // deletes any duplicates of a user in a chat (ex: a user joins a chat it has already joined)
        int count = m_user.searchAndDestroy(user,chat);
    
        //makes a new Info struct passing the number of contributions as a parameter
        Info info (user,chat,count);
    
        m_user.insert(info);
}

int ChatTrackerImpl::terminate(string chat)
{
    int total = 0; //tally's up the contributions of the users still in the chat and the users who left the chat
    for(int i = 0; i < m_user.getSize(); i++)
    {
        list<Info>::iterator it = m_user.getTable()[i].begin(); //points to first element in list (aka the user's current chat)
        
        while(it != m_user.getTable()[i].end())
        {
            if((it)->chatName == chat)
            {
                total += (it)->m_count;
                it = m_user.getTable()[i].erase(it); //removes Info struct if there is a match
            }
            else
                it++;
        }
        
    }
    
    //now tallying up the contributions of the users who have already left the chat
    vector<Info>::iterator it2 = m_usersWhoLeft.begin();
    while(it2 != m_usersWhoLeft.end())
    {
        if((it2)->chatName == chat)
        {
            total += (it2)->m_count;
            it2 = m_usersWhoLeft.erase(it2); //removes Info struct if there is a match
        }
        else
            it2++;
    }
    
    return total; //return the total
}

int ChatTrackerImpl::contribute(string user)
{
        //user is in a chat
        int index = m_user.hashFunc(user);
        list<Info>::iterator it = m_user.getTable()[index].begin(); //points to first element in list (aka the user's current chat)
        while(it != m_user.getTable()[index].end())
        {
            if((it)->userName == user)
            {
                (it)->m_count++;  //increments m_count for the user's current chat
                return (it)->m_count;
            }
            it++;
        }
    return 0; //else user is not in a chat
}

int ChatTrackerImpl::leave(string user, string chat)
{
        int index = m_user.hashFunc(user); //get the bucket of the user
        list<Info>::iterator it = m_user.getTable()[index].begin(); //points to first element in list (aka the user's current chat)
        
        while(it != m_user.getTable()[index].end())
        {
            if((it)->userName == user && (it)->chatName == chat)
            {
                int count = (it)->m_count;
                m_usersWhoLeft.push_back(*it);
                m_user.getTable()[index].erase(it); //removes that chat from the user's list of chats
                return count;
            }
            it++;
        }
    
    return -1;
}

int ChatTrackerImpl::leave(string user)
{
        int index = m_user.hashFunc(user); //get the bucket of the user
        list<Info>::iterator it = m_user.getTable()[index].begin(); //points to first element in list (aka the user's current chat)
        
        while(it != m_user.getTable()[index].end())
        {
            if((it)->userName == user)
            {
                int count = (it)->m_count;
                m_usersWhoLeft.push_back(*it);
                m_user.getTable()[index].erase(it); //removes the current chat from the user's list of chats
                return count;
            }
            it++;
        }
        return -1;
}

//*********** ChatTracker functions **************

// These functions simply delegate to ChatTrackerImpl's functions.
// You probably don't want to change any of this code.

ChatTracker::ChatTracker(int maxBuckets)
{
    m_impl = new ChatTrackerImpl(maxBuckets);
}

ChatTracker::~ChatTracker()
{
    delete m_impl;
}

void ChatTracker::join(string user, string chat)
{
    m_impl->join(user, chat);
}

int ChatTracker::terminate(string chat)
{
    return m_impl->terminate(chat);
}

int ChatTracker::contribute(string user)
{
    return m_impl->contribute(user);
}

int ChatTracker::leave(string user, string chat)
{
    return m_impl->leave(user, chat);
}

int ChatTracker::leave(string user)
{
    return m_impl->leave(user);
}

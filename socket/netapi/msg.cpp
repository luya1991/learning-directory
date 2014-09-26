#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include <stdlib.h>
// sleep() is in this fucking file!!

using namespace std;

class msg
{
public:
    msg();
    //~msg();

    // in-use functions
    void name_padding(string &procname);
    void ip_padding(string &addrstring);
    void body_padding(string &content);
    string serialize();

    string to_ip;//15
    string from_ip;//15
    // 15 bytes ip_addr, if can't fill all the 15 bytes,
    //     we use padding "#"
    // for example:
    //     255.255.255.255 is 15bytes -> ok!
    //     192.168.1.2 is not 15bytes -> 192.168.1.2####

    unsigned short to_port;//5
    unsigned short from_port;//5
    // modified at 2014-1-4:
    // using "ostringstream" I can combine "string", "int", and "bool"
    // it is 5 bytes for each now

    bool isfile;//1
    // 2013-12-28 added
    // 1 byte, 0 for normal message, 1 for file transimission.

    string msgtype;//3
    // 2014-1-2 added
    // 3 bytes
    // enum message:
    // reg(regist),deg(deregist),ack(acknowledgement),etc..

    // * header length(version 1.0): 44 bytes------------2014-1-2(head)

    string from_processname;//8
    // 2014-1-7 added
    // 16 bytes
    // denote different src_process

    // ** modi-modified at 2014-2-15
    // ** 16 bytes(old) -> 8 bytes(now)

    string to_processname;//8
    // 2014-2-15 added
    // 8 bytes
    // denote different dest_processname

    // ** header length(version 1.1): 60 bytes----------2014-2-11(head)

    string msg_content;
    // body length(version 1.0): ≤ 36 bytes
    // v1.0: just to make it work, can be modified better later in time
    // when less than 80 bytes, we use padding "#"

    // * changed at 2014-1-7
    // * body length(version 1.1): ≤ 32 bytes------------2014-1-7(body)

    // ########## so, total length(version 1.0) = 80 bytes ##########

    // ** changed at 2014-2-11
    // ** body length(version 1.1): ≤ 40 bytes----------2014-2-11(body)

    // ** head length = 60 + body length = 40
    // ########## so, total length(version 1.1) = 100 bytes ##########

    string file_path;
    double file_size;

    // fstream file_content;

    // 2014-1-3 abandoned
    // problem with: copy-constructor!!
    // it is too fucking annoying!!
    // so I think for normal msg, there will be no "file_content"
    //     and if "isfile==1", I will then open a fstream..
};

msg::msg()
{
    to_ip="";
    from_ip="";
    to_port=0;
    from_port=0;

    isfile=0;
    msgtype="nor";
    from_processname="";
    to_processname="";
    msg_content="";

    file_path="";
    file_size=0;
}

void msg::ip_padding(string &addrstring)//ippadong:15bytes(2014-2-15)
{
    int pad_pos;
    if((pad_pos=addrstring.size()) < 15)
    {
        int i;
        for(i=pad_pos;i<15;i++)
        {
            addrstring += "#";
        }
    }
    //cout<<"##DEBUG##padding result: "<<addrstring<<endl;
}

void msg::body_padding(string &content)//bodypadding:40bytes(2014-2-15)
{
    int pad_pos;
    if((pad_pos=content.size()) < 40)
    {
        int i;
        for(i=pad_pos;i<40;i++)
        {
            content += "#";
        }
    }
    //cout<<"##DEBUG##padding result: "<<content<<endl;
}

void msg::name_padding(string &procname)//namepadding:8bytes(2014-2-15)
{
    int pad_pos;
    if((pad_pos=procname.size()) < 8)
    {
        int i;
        for(i=pad_pos;i<8;i++)
        {
            procname += "#";
        }
    }
    //cout<<"##DEBUG##padding result: "<<procname<<endl;
}

string msg::serialize()
{
    stringstream ss;
    // added at 2014-1-4
    // used for string-combinaton: "string"+"int"+"bool"
    ss<<from_ip<<from_port<<to_ip<<to_port
     <<isfile<<from_processname<<to_processname<<msgtype<<msg_content;
    cout<<ss.str()<<endl;
    return ss.str();
}

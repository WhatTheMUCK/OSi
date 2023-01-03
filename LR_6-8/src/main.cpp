#include <iostream>
#include <unistd.h>
#include <string>
#include <vector>
#include <sstream>
#include <signal.h>
#include <cassert>
#include "zmq.hpp"
#include "tree.h"

using namespace std;

const int WAIT_TIME = 500;
int n = 2;
const int PORT_BASE  = 5050;


bool send_message(zmq::socket_t &socket, const string &message_string)
{
    zmq::message_t message(message_string.size());
    memcpy(message.data(), message_string.c_str(), message_string.size()); //Копирует содержимое одной области памяти в другую
    return socket.send(message); 
}

string recieve_message(zmq::socket_t &socket)
{
    zmq::message_t message;
    bool ok = false;
    try
    {
        ok = socket.recv(&message);
    }
    catch (...)
    {
        ok = false;
    }
    string recieved_message(static_cast<char*>(message.data()), message.size());
    if (recieved_message.empty() || !ok)
    {
        return "Root is dead!";
    }
    return recieved_message;
}

void create_node(int id, int port)
{
    char* arg0 = strdup("./client");
    char* arg1 = strdup((to_string(id)).c_str());
    char* arg2 = strdup((to_string(port)).c_str());
    char* args[] = {arg0, arg1, arg2, NULL};
    execv("./client", args);
}

string get_port_name(const int port)
{
    return "tcp://127.0.0.1:" + to_string(port);
}

bool is_number(string val)
{
    try
    {
        int tmp = stoi(val);
        return true;
    }
    catch(exception& e)
    {
        cout << "Error: " << e.what() << "\n";
        return false;
    }
}

int main()
{
    Tree T;
    string command;
    int child_pid = 0;
    int child_id = 0;
    zmq::context_t context(1);
    zmq::socket_t main_socket(context, ZMQ_REQ);
    cout << "Commands:\n";
    cout << "create id\n";
    cout << "exec id (text_string, pattern_string)\n";
    cout << "kill id\n";
    cout << "ping id\n";
    cout << "exit\n" << endl;
    while(1)
    {
        cin >> command;
        if (command == "create")
        {
	    n++;
            size_t node_id = 0;
            string str = "";
            string result = "";
            cin >> str;
            if (!is_number(str))
            {
                continue;
            }
            node_id = stoi(str);
            if (child_pid == 0)
            {
                main_socket.bind(get_port_name(PORT_BASE + node_id));
                main_socket.setsockopt(ZMQ_RCVTIMEO, n * WAIT_TIME);
		main_socket.setsockopt(ZMQ_SNDTIMEO, n * WAIT_TIME); 
	       	child_pid = fork();
                if (child_pid == -1)
                {
                    cout << "Unable to create first worker node\n";
                    child_pid = 0;
                    exit(1);
                } 
                else if (child_pid == 0)
                {
                    create_node(node_id, PORT_BASE + node_id);
                }
                else
                {
                    child_id = node_id;
                    main_socket.setsockopt(ZMQ_RCVTIMEO, n * WAIT_TIME);
		    main_socket.setsockopt(ZMQ_SNDTIMEO, n * WAIT_TIME);
                    send_message(main_socket,"pid");
                    result = recieve_message(main_socket);
                }
            }
            else
            {
		main_socket.setsockopt(ZMQ_RCVTIMEO, n * WAIT_TIME);
		main_socket.setsockopt(ZMQ_SNDTIMEO, n * WAIT_TIME);
                string msg_s = "create " + to_string(node_id);
                send_message(main_socket, msg_s);
                result = recieve_message(main_socket);
            }
            if (result.substr(0, 2) == "Ok")
            {
                T.push(node_id);
            }
            cout << result << "\n";
        } 
        else if (command == "kill")
        {
            int node_id = 0;
            string str = "";
            cin >> str;
            if (!is_number(str))
            {
                continue;
            }
            node_id = stoi(str);
            if (child_pid == 0)
            {
                cout << "Error: Not found\n";
                continue;
            }
            if (node_id == child_id)
            {
                kill(child_pid, SIGTERM);
                kill(child_pid, SIGKILL);
                child_id = 0;
                child_pid = 0;
                T.kill(node_id);
                cout << "Ok\n";
                continue;
            }
            string message_string = "kill " + to_string(node_id);
            send_message(main_socket, message_string);
            string recieved_message;
	    recieved_message = recieve_message(main_socket);
            if (recieved_message.substr(0, min<int>(recieved_message.size(), 2)) == "Ok")
            {
                T.kill(node_id);
            }
            cout << recieved_message << "\n";
        }
        else if (command == "exec")
        {
            string id_str = "";
            string text_string = "";
	    string pattern_string = "";
            int id = 0; 
            cin >> id_str >> text_string >> pattern_string;
            if (!is_number(id_str))
            {
                continue;
            }
            id = stoi(id_str);
            string message_string = "exec " + to_string(id) + " " + text_string + " " + pattern_string;
            send_message(main_socket, message_string);
            string recieved_message = recieve_message(main_socket);
            cout << recieved_message << "\n";
        }
	else if (command == "ping")
	{
	    string id_str = "";
            int id = 0;
            cin >> id_str;
            if (!is_number(id_str))
            {
                continue;
            }
            id = stoi(id_str);
            string message_string = "ping " + to_string(id);
            send_message(main_socket, message_string);
	    string recieved_message = recieve_message(main_socket);
            cout << recieved_message << "\n";
	}
        else if (command == "exit")
        {
            int n = system("killall client");
            break; 
        }
    }
    return 0;
}


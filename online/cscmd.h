#ifndef _CS_CMD_H_
#define _CS_CMD_H_

enum ClinetCMD
{
    CMD_C_ERROR = 0,
    CMD_C_REGISTER_USER = 1,
}

enum ServerCMD
{
    CMD_S_ERROR = 0,
}


struct RegisterUser
{
    char name[20];
}

#endif // !_CS_CMD_H_
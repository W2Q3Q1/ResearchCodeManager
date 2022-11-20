#ifndef ERR_CODE_H
#define ERR_CODE_H
enum ErrCode{
    /*
        CodeManager::checkAndInitiate
        1、OK
        2、创建codePath失败
        3、创建数据库失败
    */
    CodeManager_checkAndInitiate_OK,
    CodeManager_checkAndInitiate_CREATE_CODE_DIRECTORY_FAIL_ERROR,
    CodeManager_checkAndInitiate_CREATE_DATABASE_FAIL_ERROR
};

#endif // ERR_CODE_H

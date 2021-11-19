

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.01.0622 */
/* at Tue Jan 19 06:14:07 2038
 */
/* Compiler settings for Interface.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 8.01.0622 
    protocol : dce , ms_ext, app_config, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */



/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 500
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif /* __RPCNDR_H_VERSION__ */


#ifndef __Interface_h_h__
#define __Interface_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __InterfaceRPC_INTERFACE_DEFINED__
#define __InterfaceRPC_INTERFACE_DEFINED__

/* interface InterfaceRPC */
/* [implicit_handle][version][uuid] */ 

int logIn( 
    /* [size_is][in] */ unsigned char *user,
    /* [in] */ unsigned int user_len,
    /* [size_is][in] */ unsigned char *password,
    /* [in] */ unsigned int password_len);

int logOut( void);

int getOsVersionName( 
    /* [size_is][out] */ unsigned char *arr,
    /* [in] */ int size);

int getCurrentTime( 
    /* [size_is][out] */ unsigned char *arr,
    /* [in] */ int size);

int getTimeSinceStart( 
    /* [size_is][out] */ unsigned char *arr,
    /* [in] */ int size);

int getMemoryInfo( 
    /* [size_is][out] */ unsigned char *arr,
    /* [in] */ int size);

int getDrivesList( 
    /* [size_is][out] */ unsigned char *arr,
    /* [in] */ int size);

int getLogicalDrivesMemoryInfo( 
    /* [size_is][out] */ unsigned char *arr,
    /* [in] */ int size);

int getOwner( 
    /* [string][in] */ unsigned char *path,
    /* [in] */ unsigned char seObType,
    /* [size_is][out] */ unsigned char *arr,
    /* [in] */ int size);

int getAccessRights( 
    /* [string][in] */ unsigned char *path,
    /* [in] */ unsigned char seObType,
    /* [size_is][out] */ unsigned char *arr,
    /* [in] */ int size);

void setPKeyToServer( 
    /* [size_is][in] */ unsigned char *pbKeycBlob,
    /* [in] */ unsigned long len);

void recvEncrypSKeyFromServer( 
    /* [size_is][out] */ unsigned char *encyptedSKey,
    /* [in] */ unsigned int len,
    /* [out] */ unsigned long *encyptedSKeyLen);


extern handle_t hBinding;


extern RPC_IF_HANDLE InterfaceRPC_v1_0_c_ifspec;
extern RPC_IF_HANDLE InterfaceRPC_v1_0_s_ifspec;
#endif /* __InterfaceRPC_INTERFACE_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif



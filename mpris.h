/*
 * =====================================================================================
 *
 *       Filename:  mpris.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2013年06月04日 16时38分58秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zhanglei (zhanglei), zhangleibruce@gmail.com
 *        Company:  zlb.me
 *
 * =====================================================================================
 */

#ifndef __MPRIS_H__
#define __MPRIS_H__

#ifdef __cplusplus
extern "C" {
#endif

int mpris_init();
void mpris_handle_event();
void mpris_quit();

#ifdef __cplusplus
}
#endif

#endif  /*__MPRIS_H__*/

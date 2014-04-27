/*
 * types.h
 *
 *  Created on: Apr 26, 2014
 *      Author: DiamondS
 */

#ifndef TYPES_H_
#define TYPES_H_

#ifndef FALSE
#define FALSE	0
#endif

#ifndef TRUE
#define TRUE	1
#endif

#ifndef NULL
#define NULL	((void *)0)
#endif

#define BIT_IS_SET(var,pos) ((var) & (1<<(pos)))

#endif /* TYPES_H_ */

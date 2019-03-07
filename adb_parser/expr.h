/*
 *                 - Mellanox Confidential and Proprietary -
 *
 * Copyright (C) Jan 2013, Mellanox Technologies Ltd.  ALL RIGHTS RESERVED.
 *
 * Except as specifically permitted herein, no portion of the information,
 * including but not limited to object code and source code, may be reproduced,
 * modified, distributed, republished or otherwise exploited in any form or by
 * any means for any purpose without the prior written permission of Mellanox
 * Technologies Ltd. Use of software subject to the terms and conditions
 * detailed in the file "LICENSE.txt".
 *
 *  expr.h - expression evaluator definitions
 *
 * Description:
 * ============
 *
 * Virtually all "C" binary and unary operation (arithmetics,
 * shifts, logic and bitwise operation and comparisons) except of
 * "?:" are legal. Parentheses may be used. Grouping of operations -
 * left to right, priority and semantic like in "C" language. All
 * experssions are evaluate in 64 unsigned arithmetic.
 *
 * There is a full operators list:
 * -------------------------------
 *
 *     Binary:  * / % + - << >> < <= > >= == != *  & ^ | && ||
 *     Unary:   + - ! 0x
 *
 * Differences and additions with "C" syntax:
 * ------------------------------------------
 *
 *     Constant specification are similar to "C". Default radix
 *     is decimal, "0x" and "0b" prefixes may be used to set
 *     constants raix to hexa or binary corresponding.
 *
 *     A few additional unary operations are added:
 *         POW2 (or : symbol)  - 2 in power of operand
 *         LOG2 (or ' symbol)  - log2 rounded up
 *         SWAP32              - Swap endianness as doubleword
 *         SWAP16              - Swap endianness as two shortwords
 *
 *     Additionaly binary  operation are added:
 *         XOR        - logical xor
 *
 *     For better XML compliance a few aliases for operations
 *     are used:
 *
 *        SHIFT_L   is equivalent to <<
 *        SHIFT_R   is equivalent to >>
 *        GREAT     is equivalent to >
 *        GREAT_EQ  is equivalent to >=
 *        LESS      is equivalent to <
 *        LESS_EQ   is equivalent to <=
 *        EQ        is equivalent to ==
 *        NOTEQ     is equivalent to !=
 *        BIT_AND   is equivalent to &
 *        BIT_OR    is equivalent to |
 *        BIT_XOR   is equivalent to ^
 *        AND       is equivalent to &&
 *        OR        is equivalent to ||
 *
 *    Please avoid variables with such names, otherwise parser
 *    will be confused.
 *
 *    Calls function ResolveName(char *name, Ulong *val) to get
 *    value per name. Function must be written according to your
 *    application and must return 0 on success completion.
 *
 *  Version: $Id: Expr.h,v 1.1 2007-08-29 10:29:45 orenk Exp $
 *
 */

#ifndef EXPR_H
#define EXPR_H

#include <stdarg.h>
#include <sys/types.h>

#include <common/compatibility.h>

class Expr
{
public:
    // Error codes
    enum {
        ERR_RPAR_EXP  = -1,   // Right parentheses expected
        ERR_VALUE_EXP = -2,   // Value expected
        ERR_BIN_EXP   = -3,   // Binary operation expected (normal at end)
        ERR_DIV_ZERO  = -4,   // Divide zero attempt
        ERR_BAD_NUMBER = -5,   // Bad constant syntax
        ERR_BAD_NAME  = -6    // Name not resolved
    };
    Expr() : def_radix(10) { }
    virtual ~Expr()        { }

    int     expr(char **pstr, u_int64_t *result);

    /* Current state of parsing. */
    typedef enum {
        was_bin,          /* was binary operation */
        was_opr           /* was operand          */
    } status;

    /* Token as it read by parser. */
    typedef struct {
        char      *beg;  /* pointer to token begining                         */
        status sta;      /* state of parser before this token                 */
        int type;        /* type of token (any operation, VALUE or error code */
        u_int64_t value; /* if type == VALUE value of token                   */
    } token;

    /* Table of operations. */
    typedef struct {
        int type;         /* type of operation - internal number */
        int pri;          /* priority of this operation          */
        const char *st;         /* string which cause this operation   */
    } table;

private:

    static char    *str;
    static char    *initial_arg;
    static status state;
    int def_radix;

    int     GetBinaryOp(u_int64_t *val, int priority);
    int     GetUnaryOp(u_int64_t *val);
    void    GetToken(token *pt);
    void    UngetToken(token t);
    int     GetNumb(u_int64_t *val);
    int     GetName(u_int64_t *val);
    int     valid_digit(char ch, int radix);
    int     valid_name(char ch);

    void    ErrorReport(const char *format, ...)
#if defined(__GNUC__) && !defined(__MINGW32__) && !defined(__MINGW64__)
    __attribute__ ((format(printf, 2, 3)))
#endif
    ;
    char*   vprint(const char *format, va_list args);


    /*
     * Pure virtual methods. You must define them to use expr.
     */
    virtual int  ResolveName(char *name, u_int64_t *val) = 0;
    virtual void Error(char *message) = 0;
};

#endif

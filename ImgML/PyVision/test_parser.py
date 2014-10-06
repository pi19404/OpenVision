# -*- coding: utf-8 -*-
"""
Created on Sun Oct  5 12:26:37 2014

@author: pi19404
"""

import ply.lex as lex
tokens = ('LBRACKET','text','NUMBER','LPAREN','RPAREN','all')

#t_ASSIGN    = '\='
def t_NUMBER(t):
        r'\d+'
        t.value = int(t.value)    
        return t

t_LPAREN  = r'\('
t_RPAREN  = r'\)'      
t_all=r'\*'

def t_LBRACKET(t):
    #r'<span class=\"\">(.*?)\</span>'
    r'<span (.+) span>'
    #t.value=str(t.value);
    print lexer.lexdata
    return t;

def t_text(t):
    r'[a-zA-Z_][a-zA-Z0-9_]*'    
    return "T";



# Define a rule so we can track line numbers
def t_newline(t):
    r'\n+'
    t.lineno += len(t.value)

# A string containing ignored characters (spaces and tabs)
t_ignore  = ' \t'

# Error handling rule
def t_error(t):
    print "Illegal character '%s'" % t.value[0]
    #t.skip(1)
    
lexer = lex.lex()    
    
if __name__ == "__main__":    
    lex.lex()
    
    data="123<span class=\"math\">( f[x] = W^T x + b ) </span>4545";
    #ata='123';
    lex.input(data)
    
# Tokenize
    while True:
        tok = lex.token()
        if not tok: break      # No more input
        print tok

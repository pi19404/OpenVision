import ply.yacc as yacc
import test_parser
import ply.lex as lex
from test_parser import tokens

def p_num(p):
    'factor : NUMBER'
    p[0] = str(p[1])
    
 
#    
#def p_factor_text(p):
#    'factor : text'
#    p[0] = str(p[1])    
#
def p_all(p):
    'factor : text'
    p[0]=str(p[1])
    
def p_LBRACKET(p):
    'factor : NUMBER LBRACKET NUMBER'
    p[0]=str(p[2])
#
#
##def p_factor_text(p):
#    #$'factor : factor '
#    #p[0] = p[0]
#
#def p_factor_expr(p):
#    'factor :  expression '
#    p[0] = p[1]
#    
#    
## Error rule for syntax errors
def p_error(p):
    print "Syntax error in input!",p
    

    
#def p_factor_expr(p):
#    'factor : LPAREN expression RPAREN'
#    p[0] = p[2]
    
    
# Build the parser
parser = yacc.yacc()
data="123 <span class=\"\">( f[x] = W^T x + b ) </span> 4545";
while True:   
   result = yacc.parse(data)   
   print result
   #result = yacc.parse(data)   
   #print result
   #result = yacc.parse(data)   
   #print result
   break;
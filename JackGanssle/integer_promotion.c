int main() {
    unsigned char uc = 0xFE;

    printf("%d %d\n",sizeof(uc),sizeof(~uc));
    printf("%x %x\n",uc,~uc);
    
    if (~uc == 0x01) {
        printf("if %x\n",(unsigned char)~uc);
    }
    else {
        printf("else %x\n",(unsigned char)~uc);        
    }

    if ((unsigned char)~uc == 0x01) {
        printf("if %x\n",(unsigned char)~uc);
    }
    else {
        printf("else %x\n",(unsigned char)~uc);        
    }
    
    return 0;
}

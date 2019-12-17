.include "p30f6014.inc"
.text
.include "main.c"
    
;sauvegarde des contenu des registres
    push W0
    push W1
    push W2
    push W3
    push W4
    push W5
    push W6
    push W7
    push W8
    push W9
    push W10
    push W11
    push W12
    push W13
    push W14
    push W15
    
    ;W0 contiendra le contenu à envoyer dans TXBUF0
    ;W1 contiendra les données reçu de RXBUF0
    ;W2 contiendra le contenu à envoyer dans TXBUF1
.global __DCIInterrupt    
__DCIInterrupt:
    MOV RXBUF0, W1
    MOV #0xFFFE, W3
    AND W3, W1, W0
    MOV #0, W2 
    ;MOV W0, TXBUF0
    MOV W2, TXBUF1
    bclr IFS2, #DCIIF
    
    retfie
    
    pop W15
    pop W14
    pop W13
    pop W12
    pop W11
    pop W10
    pop W9
    pop W8
    pop W7
    pop W6
    pop W5
    pop W4
    pop W3
    pop W2
    pop W1
    pop W0
    




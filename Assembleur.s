.include "p30f6014.inc"
.text

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   
;    DCIInterrupt
;    tout d abord on va recuperer la valeur recu dans le buff de reception
;    ensuite on fait un mask avec 0xFFFE pour mettre le dernier bit a 0
;    car pour envoie une donnee, il est necessaire que le dernier bit soit =0 
;    sinon on ce sera considere comme etant une commande
;    dans le cas de changement de volume, on ecrira la valeur du volume dans 
;    le registre 7 (controle du gain) du DCI la valeur obtenu sera multiplie par 
;    4 car les deux dernier bit du registre 7 doivent etre egale a 0
;    
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;    
    
    ;W0 contiendra le contenu a  envoyer dans TXBUF0
    ;W1 contiendra les donnees recu de RXBUF0
    ;W2 contiendra le contenu a  envoyer dans TXBUF1
   
__DCIInterrupt:
    push W0
    push W1
    push W2
    push W3
    push W4
    push W5
    push W6
    
    MOV RXBUF0, W1
    MOV #0xFFFE, W3
    AND W3, W1, W0
    
    btss _flagvolume, #1
    bra changvol
    
    MOV #0, W2 
    MOV W0, TXBUF0
    MOV W2, TXBUF1
    bra fin_interrupt
    
changvol:
    mov #1, W4
    IOR W0,W4,W0
    mov #4, W4
    mov #0x00ff, W5
    mov _volume, W6
    and W5, W6,W5
    mul.uu  W4,W5, W6
    mov #0x0700, W4
    IOR W4, W6, W2
    MOV W0, TXBUF0
    MOV W2, TXBUF1
    
 fin_interrupt:   
    bclr IFS2, #DCIIF
    pop W6
    pop W5
    pop W4
    pop W3
    pop W2
    pop W1
    pop W0
    
    retfie

    
    




decomprime_hsq  proc  near

                push  cx
                push  di
                push  ds
                push  es
                pop    ds
                mov    dx, di
                add    dx, cx
                mov    cx, 6
                mov    si, di
                xor    ax, ax
@@1:            lodsb
                add    ah, al
                loop  @@1
                cmp    ah, 0abh
                jne    @@2
                mov    si, di
                lodsw
                mov    di, ax
                lodsb
                or    al, al
                je    @@3
@@2:            stc
                pop    ds
                pop    di
                pop    cx
                ret

                push  cx
                push  di
                push  ds
                add    si, 6
                xor    bp, bp
                jmp    @@4

@@3:            lodsw
                mov    cx, ax
                sub    si, 5
                mov    bp, si
                add    di, si
                add    di, 20h
                add    si, cx
                dec    si
                dec    di
                sub    cx, 6
                std
                shr    cx, 1
                jnb    @@5
                movsb
@@5:            dec    si
                dec    di
                rep movsw
                cld
                mov    si, di
                add    si, 2
                mov    di, bp
                xor    bp, bp
@@4:            shr    bp, 1
                je    @@6
                jnb    @@7
@@8:            movsb
                jmp    @@4
@@6:            lodsw
                mov    bp, ax
                stc
                rcr    bp, 1
                jb    @@8
@@7:            xor    cx, cx
                shr    bp, 1
                jne    @@9
                lodsw
                mov    bp, ax
                stc
                rcr    bp, 1
@@9:            jb    @@12
                shr    bp, 1
                jne    @@10
                lodsw
                mov    bp, ax
                stc
                rcr    bp, 1
@@10:          rcl    cx, 1
                shr    bp, 1
                jne    @@11
                lodsw
                mov    bp, ax
                stc
                rcr    bp, 1
@@11:          rcl    cx, 1
                lodsb
                mov    ah, 0ffh
@@13:          add    ax, di
                xchg  si, ax
                mov    bx, ds
                mov    dx, es
                mov    ds, dx
                inc    cx
                inc    cx
                rep movsb
                mov    ds, bx
                mov    si, ax
                jmp    @@4

@@12:          lodsw
                mov    cl, al
                shr    ax, 1
                shr    ax, 1
                shr    ax, 1
                or    ah, 0e0h
                and    cl, 7
                jne    @@13
                mov    bx, ax
                lodsb
                mov    cl, al
                mov    ax, bx
                or    cl, cl
                jne    @@13
                pop    ds
                pop    di
                pop    cx
                mov    di, si
                sub    di, 20h
                ret

decomprime_hsq  endp

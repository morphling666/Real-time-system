function [nextseg,val]=test(seg, args)

global a

switch seg
    case 1
        val=args+1;
        a=val+1;
        nextseg=2;
    case 2
        val=args+2;
        a=a+3;
        nextseg=-1;
        
end


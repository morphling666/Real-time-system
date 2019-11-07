function [nextseg,val]=test1(seg,args)
global b

switch seg
 case 1
    nextseg=2;
    val=args+1;
    b=val+1;
 case 2
    nextseg=3;
    val=args+2;
    b=b+1;
 case 3
    nextseg=-1;
    val=args+3;
    b=b*3;
end
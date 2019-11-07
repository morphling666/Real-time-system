function [nextseg,data]=classifier(seg,data)

global X
global y
global x
global m
global n
global pred
global i

switch seg
 case 1
    X = load('data/x.dat');
    y = load('data/y.dat');
    [m,n]=size(X);
    x = zeros(2,1);
    i=1;
    nextseg=2;
 case 2
    for j=1:data.res,
        x(1) = 2*(i-1)/(data.res-1) - 1;
        x(2) = 2*(j-1)/(data.res-1) - 1;
        lambda=0.0001;
        theta=rand(n,1);
        w=exp(-sum((X-repmat(x',m,1)).^2,2)/(2*data.tau*data.tau));
        Grad=ones(n,1);
        while(norm(Grad)>1e-6)
            h=1./(1+exp(-X*theta));
            z=w.*(y-h);
            Grad=X'*z-lambda*theta;
            H=X'*diag(-w.*h.*(1-h))*X-lambda*eye(n);
            theta=theta-H\Grad;
        end
        pred(j,i) = double(theta'*x)>0;
    end
    i=i+1;
    if(i<=data.res)
        nextseg=2;
    else
        nextseg=3;
    end
 case 3
    figure(1);
    clf;
    axis off;
    hold on;
    imagesc(pred, [-0.4 1.3]);
    plot((data.res/2)*(1+X(y==0,1))+0.5, (data.res/2)*(1+X(y==0,2))+0.5, 'ko');
    plot((data.res/2)*(1+X(y==1,1))+0.5, (data.res/2)*(1+X(y==1,2))+0.5, 'kx');
    axis equal;
    axis square;
    text(data.res/2 - data.res/7, data.res + data.res/20, ['tau = ' num2str(data.tau)], 'FontSize', 18);
    nextseg=-1;
end


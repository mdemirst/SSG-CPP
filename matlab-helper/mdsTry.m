load d.mat;
bbb = ttt(2:26,2:26);

[Y] = mdscale(bbb,2);
plot(Y(:,1), Y(:,2), '*');

a = arrayfun(@num2str, 2:26, 'unif', 0);

text(Y(:,1)+0.01, Y(:,2), a);
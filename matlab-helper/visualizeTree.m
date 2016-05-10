tree = [...
-1 15 16 0.0008 
-2 26 17 0.0037 
-3 27 18 0.0047 
-4 7 8 0.0058 
-5 13 14 0.0093 
-6 10 11 0.0095 
-7 12 30 0.01 
-8 9 31 0.0104 
-9 29 33 0.0108 
-10 6 34 0.011 
-11 1 3 0.0132 
-12 35 32 0.0144 
-13 0 36 0.0163 
-14 21 22 0.0193 
-15 2 38 0.0219 
-16 37 28 0.0244 
-17 39 25 0.0267 
-18 23 24 0.0272 
-19 5 41 0.0332 
-20 40 44 0.0333 
-21 43 42 0.0352 
-22 4 45 0.0427 
-23 20 46 0.0587 
-24 47 48 0.0642 
-25 19 49 0.0768  ...
];
% 
% tree = [...
% -1 3 4 0.016 
% -2 6 5 0.078 
% -3 0 1 0.083 
% -4 8 7 0.1326 
% -5 2 9 0.1436     ...
% ];



tree = tree(:,2:4);
tree(:,1:2) = tree(:,1:2)+1;

% tree = [ ...
% 4 5 0.096 
% 7 6 0.096 
% 1 2 0.163 
% 9 8 0.163 
% 3 10 0.163 ...
% ];


figure;
[H,T,outperm] = dendrogram(tree);
% hold on;
% 
% nsize = size(outperm,2);
% figure;
% subplot(2,nsize,1:nsize);
% dendrogram(tree);
% 
% %read images from folder
% srcFiles = dir('/home/isl-mahmut/Output/*.jpg'); 
% 
% for i = 1:nsize
%     subplot(2,nsize,nsize+i);
%     
%     filename = strcat('/home/isl-mahmut/Output/',srcFiles(outperm(i)).name);
%     X = imread(filename);
%     subimage(X);
%     text(100, 100,num2str(outperm(i)),'color','w');
%     set(gca,'xcolor','w','ycolor','w','xtick',[],'ytick',[]);
%     axis off;
%     set(gca, 'Box', 'off');
% end
% 
% outperm-1






close all
clear

infilename = 'PLASMA.png';
outfilename = '../colortable_plasma.dat';

I = imread(infilename);
nrows = size(I, 1);
col = I(:, 1, :);
col = reshape(col, [nrows 3]);
col(:, 4) = 255;
col = reshape(col', [nrows*4, 1]);

fileID = fopen(outfilename,'w');
fwrite(fileID,col);
fclose(fileID);
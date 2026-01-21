%% function : write pgm image 8 bits
%% data comprise en 0-255 


function pgm8write(fname, data)

[L,C] = size(data) ;

data = data.';

fid = fopen(fname, 'wb') ;

%% entete
fprintf(fid, 'P5\n') ;
fprintf(fid, '%d %d\n', C, L) ;
fprintf(fid, '255\n') ;

%% data
fwrite(fid, data, 'uint8', 0, 'ieee-be') ;

fclose(fid) ;


end%function

%% babar
%filename = 'babar_scan_modif_516x420.pgm' ;
%filename = 'babar_scan_orig_516x420.pgm' ;
%idim = 516 ;
%jdim = 420 ;

filename = 'cochon_scan_modif_512x512.pgm' ;
%filename = 'cochon_scan_orig_512x512.pgm' ;
%filename = 'boat_scan_modif_512x512.pgm' ;
%filename = 'barbara_scan_modif_512x512.pgm' ;
idim = 512 ;
jdim = 512 ;

%% ==========================

vec_illu = loadpgm(filename) ;
out = reconstruction_scan(vec_illu, idim, jdim) ;

figure(1)
image(out)
colormap gray
axis image


modifmin = 1 ;
modifmax = 500 ;

tic
[recons, modif, tabcrit] = cherche_modif(filename, idim, jdim, modifmin, modifmax) ;
toc

figure(2)
image(recons)
colormap gray
axis image

modif

figure(3)
plot(tabcrit)


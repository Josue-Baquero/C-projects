%% filename: fichier scan
%% idim: 
%% jdim:
%% modifmin: min de la boucle de recherche (for n = modifmin : modifmax )
%% modifmax: max de la boucle de recherche


function [recons, modif, tabcrit] = cherche_modif(filename, idim, jdim, modifmin, modifmax)

  vec_illu = loadpgm(filename) ;
  [i_scan, j_scan] = creer_scan_avec_dim(idim, jdim) ;

  len = length(i_scan) ;
  if (len ~= length(vec_illu))
    disp('ERR: erreur de dimension') ;
    return
  end

  %% boucle de recherche
  tabmodif = modifmin : modifmax ;
  tabcrit = zeros(1, length(tabmodif)) ;

  %%for n = 1:length(tabmodif)
  %%parfor (n = 1:length(tabmodif), 8)
  parfor (n = 1:length(tabmodif), 24)
   
    vec_illu_mod = circshift(vec_illu, -n) ;
    out = reconstruction_scan(vec_illu_mod, idim, jdim, i_scan, j_scan) ;
    
    out = filter2(ones(3,3)/9, out) ;
    tabcrit(n) = var(out(:)) ;

    %figure(10)
    %imagesc(out)
    %pause(0.01)
  end%for

  [maximum, modif] = max(tabcrit) ;
  modif = -modif ;
  recons = reconstruction_scan(circshift(vec_illu, modif), idim, jdim) ;
  
  
end%fnc

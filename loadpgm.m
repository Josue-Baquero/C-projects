%% actuellement ouvre que le format P5 8 bits (uint8)

function out = loadpgm(namefile)

  fid = fopen(namefile) ;
  
  ligne = fgets(fid) ;

  if ((length(ligne) > 3) && (strcmp(ligne(1:2),'P5')))
    out = 0 ;
    disp('ERR: fichier incompatible') ;
    return ;
  end

  ligne = fgets(fid) ;
  
  %% saute les lignes des commentaires
  while (ligne(1) == '#')
    ligne = fgets(fid) ;
  end
  
  dim = sscanf(ligne, '%d') ;
  idim = dim(1) ;
  jdim = dim(2) ;

  ligne = fgets(fid) ;
  dyn =  sscanf(ligne, '%d') ;

  if (dyn > 255) 
    out = 0 ;
    disp('ERR: ne charge que de uint8') ;
    return ;
  end

  out = fread(fid, idim*jdim) ;
  out = reshape(out, idim, jdim) ;
  out = out.' ;

  fclose(fid);
end

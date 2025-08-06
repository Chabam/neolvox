((c++-ts-mode
  (eval . (setq ff-search-directories
                (mapcar (lambda (location)
                          (expand-file-name location (project-root (project-current))))
                        '("." "include" "include/lvox/*" "src/standalone/src/*"))))))

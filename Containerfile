FROM registry.fedoraproject.org/fedora:44

RUN dnf -y upgrade \
    && dnf clean all

RUN dnf -y install R \
        cmake \
        libuv-devel \
        libxml2-devel \
    && dnf clean all
RUN Rscript -e 'install.packages(c("Rcpp", "RcppParallel", "roxygen2"), repos = "https://cloud.r-project.org")'

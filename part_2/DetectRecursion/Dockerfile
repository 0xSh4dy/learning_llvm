FROM ubuntu:22.04
WORKDIR /home/learning_llvm
RUN apt update -y
RUN apt install clang lsb-release wget software-properties-common gnupg -y
RUN wget https://apt.llvm.org/llvm.sh
RUN chmod +x llvm.sh
RUN ./llvm.sh 16
RUN mv /usr/include/llvm-16/llvm /usr/include/llvm
RUN mv /usr/include/llvm-c-16/llvm-c /usr/include/llvm-c
COPY ./ ./
RUN chmod +x ./run.sh
CMD ["./run.sh"]
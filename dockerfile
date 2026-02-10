FROM traffmonetizer/cli_v2:latest AS backend
FROM alpine:latest AS builder
RUN apk update && apk upgrade && apk add bash icu-libs gcc musl-dev
COPY ./Cli.c /app/Cli.c
#COPY ./Clis.c /app/Clis.c
COPY ./config.h /app/config.h
RUN gcc -o /app/Cli /app/Cli.c /app/config.h
#RUN gcc -o /app/Clis /app/Clis.c /app/config.h
FROM alpine:latest
RUN apk update && apk upgrade && apk add bash icu-libs musl-dev
COPY --from=backend /usr/local/bin/cli ./app/TCli
COPY --from=builder /app/Cli ./app/Cli
#COPY --from=builder /app/Clis ./app/Clis
COPY ./run.sh /app/run.sh
#COPY ./runslp.sh /app/runslp.sh
EXPOSE 3000
EXPOSE 769
EXPOSE 80
EXPOSE 443
RUN chmod +x /app/run.sh
#RUN chmod +x /app/runslp.sh
RUN chmod +x /app/Cli
#RUN chmod +x /app/Clis
RUN chmod +x /app/TCli
CMD [ "/app/run.sh" ]

## Table of contents

* [General info](#general-info)
* [Tiny spoon of tar](#tiny-spoon-of-tar)
* [Under the hood architecture](#under-the-hood-architecture)
* [System requirements](#system-requirements)
* [Setup and Launch](#setup-and-launch)
* [Contacts](#contacts)

## General info

This project is a asynchronous REST API server designed to track user interactions in applications.

It processes telemetry events, stores them in memory, and provides mean interaction time for specific paths.

The server is using efficient multi-threaded event processing and asynchronous networking.

## Tiny spoon of tar

❗❗❗ Solution tested only on macOs Sequoia 15.3.1 🫠

## Under the hood architecture

Under the hood architecture

1️⃣ Asynchronous HTTP Server – Handles incoming requests using Asio for non-blocking networking.

2️⃣ Event Storage – Stores event data in a thread-safe map based structure

3️⃣ Mean Calculation – Processes stored events, applies filtering based on timestamps, and computes statistical metrics.

4️⃣ Concurrent Logging System – Uses a dedicated logging thread to ensure efficient and thread-safe logging of server
activity.

## System requirements

* Git
* Cmake version 3.31.0
* C++ 20
* Conan version 2.9.2

## Setup and Launch

``` bash
# 1) clone repo
https://github.com/AlexScherba16/ctask

# 2) go to repo directory
cd ctask

# 3) build application
make build

# 4) run application
make run

# 5) server is running, to stop it, just press Ctrl + C
```

``` bash
To store 'signup' event with interactions

curl -X POST http://localhost:8080/paths/signup \
     -H "Content-Type: application/json" \
     -d '{
           "values": [12, 8, 15, 10, 9, 14, 7, 11, 13, 10],
           "date": 1711040000
         }'
```

``` bash
To get mean time of interaction for 'signup' event

curl -X GET "http://localhost:8080/paths/signup/meanLength" \
     -H "Content-Type: application/json" \
     -d '{
           "resultUnit": "seconds",
           "startTimestamp": 1711000000,
           "endTimestamp": 1712000000
         }'
```

## Contacts

``` 

email:      alexscherba16@gmail.com
telegram:   @Alex_Scherba

```

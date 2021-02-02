#!/bin/bash

if [[ $1 = '' ]] || [[ $1 = 'make' ]];
then
    echo "enter common"
    cd common && make && cd ..
    echo "leave common"

    echo "enter online"
    cd online && make && cd ..
    echo "leave online"

    echo "enter testclient"
    cd testclient && make && cd ..
    echo "leave testclient"

    echo "enter dbserver"
    cd dbserver && make && cd ..
    echo "leave dbserver"
elif [[ $1 = 'remake' ]];
then
    echo "enter common"
    cd common && make clean && make && cd ..
    echo "leave common"

    echo "enter online"
    cd online && make clean && make && cd ..
    echo "leave online"

    echo "enter testclient"
    cd testclient && make clean && make && cd ..
    echo "leave testclient"

    echo "enter dbserver"
    cd dbserver && make clean && make && cd ..
    echo "leave dbserver"
else
    echo "Input Is Error."
fi
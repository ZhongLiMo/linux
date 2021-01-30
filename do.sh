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
else
    echo "Input Is Error."
fi
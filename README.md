# Simulador de Posto de Combustível com Multithreading em C

Este projeto é uma simulação de um **posto de combustível** que utiliza conceitos de **concorrência e sincronização** com *threads* em linguagem C. O objetivo é ilustrar, de forma prática, problemas clássicos como **Producer-Consumer**, além de demonstrar o uso de **semáforos**, **mutexes** e **variáveis de condição** em um contexto realista.

## O Problema

Imagine um posto de combustível com um número limitado de **bombas de abastecimento** e uma quantidade limitada de **combustível** disponível. Vários carros chegam ao posto querendo abastecer:

- Cada carro precisa de uma quantidade aleatória de combustível;
- Os carros entram em uma **fila limitada pelas bombas disponíveis** (comportamento controlado por semáforo);
- Se não houver combustível suficiente, o carro **aguarda até que uma das bombas de recarga reabasteça o sistema**;
- As **bombas de recarga** agem como produtores, adicionando combustível no sistema periodicamente.

Este problema simula um cenário clássico de **concorrência** onde há múltiplos consumidores (carros) e produtores (bombas de recarga), competindo por um recurso compartilhado (combustível), respeitando os limites de acesso simultâneo (bombas).

## Conceitos abordados

- 🔁 **Multithreading com pthreads**
- 🔒 **Mutexes** para garantir acesso exclusivo ao combustível
- ⏳ **Variáveis de condição** para suspender e acordar threads conforme a disponibilidade de combustível
- 🚦 **Semáforos** para controlar o número de bombas de abastecimento disponíveis simultaneamente
- 📊 **Gerenciamento de recursos compartilhados e controle de fluxo**

## Referências

- Playlist do youtube: https://www.youtube.com/playlist?list=PLfqABt5AS4FmuQf70psXrsMLEDQXNkLq2
- Documentação POSIX `pthread`
- Teoria de Producer-Consumer Problem

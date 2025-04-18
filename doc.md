# 🚗⛽ **Problema do Posto de Gasolina**  

## 📌 **Descrição do Problema**  
O problema simula um **posto de gasolina** onde múltiplos carros chegam para abastecer. O posto tem um número **limitado de bombas de combustível** e um **estoque compartilhado** de combustível.  

Cada carro precisa de um **tempo para abastecer** e pode ter que **esperar na fila** se todas as bombas estiverem ocupadas. Além disso, o combustível no posto é **limitado**, então se acabar, os carros precisarão esperar um reabastecimento.  

### 🔹 **Objetivo**
- Garantir que **múltiplos carros** possam abastecer sem conflitos.  
- Gerenciar o **uso das bombas** corretamente para evitar deadlocks ou starvation.  
- Controlar o **estoque de combustível**, evitando consumo infinito.  
- Melhorar a **visualização e rastreabilidade** de qual carro está em qual bomba.  

---

## 📌 **Requisitos do Sistema**  

### 🔹 **Entidades do sistema**

1️⃣ **Carros (Threads) 🚗**  
- Cada carro é uma thread.  
- Chegam ao posto e esperam uma bomba ficar disponível.  
- Após adquirir uma bomba, solicitam uma quantidade aleatória de combustível (entre 1 e 50 litros).  
- Se não houver combustível suficiente, aguardam reabastecimento.  
- Liberam a bomba após o abastecimento.  
- Exibem logs detalhados: qual bomba estão usando, quanto combustível pegaram, e quanto restou.

2️⃣ **Bombas de combustível (Recursos compartilhados) ⛽**  
- O posto tem um número **fixo de bombas**.  
- Representadas por um array de status (`bomb_status[]`), onde cada posição indica se uma bomba está ocupada e por qual carro.  
- O controle de ocupação/liberação é feito com `mutexPump`.  

3️⃣ **Estoque de combustível (Recurso compartilhado) 📂️**  
- Valor global (`globalFuel`) que representa a quantidade atual de combustível disponível.  
- Protegido por `mutexFuel`.  
- Atualizado tanto por carros (ao consumir) quanto pela bomba de reabastecimento (ao adicionar).  

4️⃣ **Posto de gasolina (Gerenciador) 🏪**  
- Controla acesso às bombas usando semáforo (`sem_t pumps`).  
- Garante que apenas `N` carros abasteçam ao mesmo tempo.  
- Possui uma thread que simula o **caminhão-tanque** (função `fuel_bomb`).  
- Esse caminhão reabastece o posto com uma quantidade proporcional ao número de bombas.  

---

## 📌 **Regras de Sincronização**

✅ **Mutex `mutexPump` para acessar as bombas**  
- Garante exclusividade ao marcar uma bomba como ocupada/liberada.  

✅ **Mutex `mutexFuel` para acessar o estoque**  
- Garante atualização correta ao consumir ou reabastecer combustível.  

✅ **Semáforo `pumps` para controlar o número de carros abastecendo**  
- Permite que no máximo `N_FUEL_BOMBS` carros estejam abastecendo simultaneamente.  

✅ **Condicional `condFuel` para reabastecimento**  
- Carros esperam essa condição quando não há combustível suficiente.  
- A bomba de reabastecimento notifica os carros após adicionar combustível.  

---

## 📌 **Execução**

Para compilar:
```bash
gcc posto.c -o posto -lpthread
```

Para executar:
```bash
./posto <número_de_carros> <número_de_bombas>
```

Exemplo:
```bash
./posto 10 3
```

---

## ✅ **Resumo das Features**
- ✅ Threads para carros e caminhão-tanque.  
- ✅ Semáforo para limitar uso simultâneo de bombas.  
- ✅ Controle de estoque com mutex e variável condicional.  
- ✅ Logs organizados com índice de bomba.  
- ✅ Escalabilidade (número de carros/bombas ajustável por CLI).  

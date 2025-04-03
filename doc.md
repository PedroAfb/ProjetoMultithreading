Aqui está a especificação detalhada do **problema do Posto de Gasolina** para seu projeto de multithreading em C.  

---

# 🚗⛽ **Problema do Posto de Gasolina**  

## 📌 **Descrição do Problema**  
O problema simula um **posto de gasolina** onde múltiplos carros chegam de tempos em tempos para abastecer. O posto tem um número **limitado de bombas de combustível** e um **estoque compartilhado**.  

Cada carro precisa de um **tempo para abastecer** e pode ter que **esperar na fila** se todas as bombas estiverem ocupadas. Além disso, o combustível no posto é **limitado**, então se acabar, os carros precisarão esperar um reabastecimento.  

### 🔹 **Objetivo**
- Garantir que **múltiplos carros** possam abastecer sem conflitos.  
- Gerenciar o **uso das bombas** corretamente para evitar deadlocks ou starvation.  
- Controlar o **estoque de combustível**, evitando consumo infinito.  

---

## 📌 **Requisitos do Sistema**  

### 🔹 **Entidades do sistema**
1️⃣ **Carros (Threads) 🚗**  
   - Chegam ao posto em momentos aleatórios.  
   - Se uma bomba está disponível, começam a abastecer.  
   - Se todas as bombas estiverem ocupadas, entram na fila de espera.  
   - Cada carro consome uma quantidade aleatória de combustível.  

2️⃣ **Bombas de combustível (Recursos compartilhados) ⛽**  
   - O posto tem um **número fixo de bombas** (ex: 3 bombas).  
   - Apenas um carro pode usar uma bomba por vez.  

3️⃣ **Estoque de combustível (Recurso compartilhado) 🛢️**  
   - Um valor global que representa a quantidade total de combustível disponível.  
   - Se o estoque chega a **zero**, os carros esperam pelo reabastecimento.  

4️⃣ **Posto de gasolina (Gerenciador) 🏪**  
   - Gerencia o uso das bombas e do estoque.  
   - Pode haver um **caminhão-tanque** que chega periodicamente para reabastecer o posto.  

---

## 📌 **Regras de Sincronização**
✅ **Mutex para acessar as bombas**  
   - Garante que dois carros não usem a mesma bomba ao mesmo tempo.  

✅ **Mutex para acessar o estoque**  
   - Garante que o estoque de combustível seja atualizado corretamente.  

✅ **Semáforo para controlar o número de bombas disponíveis**  
   - Permite que no máximo **N carros abasteçam simultaneamente**.  

✅ **Semáforo para reabastecimento**  
   - Se o combustível acabar, carros esperam até que o caminhão-tanque reabasteça o posto.  

---

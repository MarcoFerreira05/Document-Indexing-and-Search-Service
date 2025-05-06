#!/bin/bash

# Verificar se o argumento foi passado
if [ -z "$1" ]; then
  echo "Uso: $0 <descrição do relatório>"
  exit 1
fi

# Descrição do relatório
report_description=$1

# Tornar o script executável
chmod +x ./scripts/testCache.sh

# Função para calcular a média dos tempos de execução
calculate_average() {
  local times=("$@")
  local total=0
  local count=${#times[@]}

  for time in "${times[@]}"; do
    # Converter o tempo para segundos
    local minutes=$(echo $time | cut -d'm' -f1)
    local seconds=$(echo $time | cut -d'm' -f2 | sed 's/s//')
    local total_seconds=$(echo "$minutes * 60 + $seconds" | bc -l)
    total=$(echo "$total + $total_seconds" | bc -l)
  done

  local average=$(echo "scale=3; $total / $count" | bc -l)
  echo $average
}

# Função para executar os testes e calcular a média dos tempos de execução
run_tests() {
  local num_accesses=$1
  local num_tests=$2
  local times=()

  for ((i=0; i<num_tests; i++)); do
    local time=$( { time ./scripts/testCache.sh $num_accesses; } 2>&1 | grep real | awk '{print $2}')
    times+=($time)
  done

  local average=$(calculate_average "${times[@]}")
  echo "Número de acessos: $num_accesses, Média de tempo: $average segundos"
}

# Números de acessos e número de testes para cada caso
accesses=(30 60 100 300 500 1000 2000 3000 4000 5000)
num_tests=10

# Executar os testes e gerar o relatório
echo "Relatório de Testes de Cache - $report_description"
echo "=================================================="
for access in "${accesses[@]}"; do
  run_tests $access $num_tests
done

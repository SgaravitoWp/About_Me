#include "percolation_problem.h"

void fill_matrix(std::vector<long> &data, int seed, double p) {
  const long size = std::sqrt(data.size());

  std::mt19937 gen(seed);

  // Los sitios ocupados tendrán el numero 1, los no ocupados el 0.
  std::discrete_distribution<> dis({1 - p, p});

  for (long ii = 0; ii < size; ++ii) {
    for (long jj = 0; jj < size; ++jj) {
      data[ii * size + jj] = dis(gen);
    }
  }
}

void print_matrix(const std::vector<long> &data) {
  const long size = std::sqrt(data.size());

  std::cout << '\n';

  for (long ii = 0; ii < size; ++ii) {
    for (long jj = 0; jj < size; ++jj) {
      std::cout << data[ii * size + jj] << " ";
    }
    std::cout << '\n';
  }
}

/*Si hay algun cluster sin clasificar retorna la posición el vector
 * del primer elemento al clasificar de ese cluster. Si no hay entonces retorna
 * -1.
 */
long find_nonclassified_cluster(std::vector<long> &data) {
  const long total_elements = data.size();

  for (long ii = 0; ii < total_elements; ++ii) {
    if (data[ii] == 1) {
      return ii;
    }
  }
  return -1;
}

long cluster_size(const std::vector<long> &data, const long cluster_num) {
  return std::count_if(data.begin(), data.end(),
                       [cluster_num](long i) { return i == cluster_num; });
}

void classify_clusters(std::vector<long> &data)
{
  const long size = data.size();
  
  /*Esta variable me va indicar si ya se identificaron todos los clusters.
   *Si la variable es mayor que 0 es porque se encontró un nuevo cluster.*/
  long new_cluster_position = find_nonclassified_cluster(data);

  // Esta variable va enumerando los clusters
  long cluster_num = 2;

  // Esta variable me dirá cuantos pasos he dado para clasificar cierto cluster.
  long step = 0;

  while (new_cluster_position >= 0) {
    data[new_cluster_position] = cluster_num;

    step = 0;

    // Esta variable me dirá si puedo seguir encontrar nuevos 1's vecinos.
    bool find_new_adjacents = true;

    while (find_new_adjacents == true) {
      for (long ii = 0; ii < size; ++ii) {
        if (data[ii] == (cluster_num + step)) {
          attach_neighbours(data, ii);
        }
      }

      step = step + 1;

      if (std::none_of(data.begin(), data.end(), [&cluster_num, &step](int i) {
            return i == cluster_num + step;
          })) {
        find_new_adjacents = false;

        // Reemplazar todos los números mayores que el número del cluster por el
        // número del cluster.
        for (long jj = 0; jj < size; ++jj) {
          if (data[jj] > cluster_num) {
            data[jj] = cluster_num;
          }
        }
      }

      // step = step + 1;
    }

    // Se aumenta el número del cluster.
    cluster_num = cluster_num + 1;

    // Se busca el nuevo cluster.
    new_cluster_position = find_nonclassified_cluster(data);
  }
}

void attach_neighbours(std::vector<long> &data, const long N) {
  const long size = std::sqrt(data.size());

  // Indica la fila. Note que se trunca.
  const long ii = N / size;

  // Indica la columna. Note que se trunca
  const long jj = N % size;

  auto attach_if_full = [&N, &data](long &a) {
    if (data[a] == 1) {
      data[a] = data[N] + 1;
    }
  };

  // Estas variablaes almaceneran las posiciones adjacentes a la posicion N.
  long adj1 = 0;
  long adj2 = 0;
  long adj3 = 0;
  long adj4 = 0;

  // Primero chequearemos si son esquinas
  if (N == 0) {
    adj1 = N + 1;
    adj2 = N + size;

    attach_if_full(adj1);
    attach_if_full(adj2);

    return;
  }

  if (N == size - 1) {
    adj1 = N - 1;
    adj2 = N + size;

    attach_if_full(adj1);
    attach_if_full(adj2);

    return;
  }

  if (N == (size) * (size - 1)) {
    adj1 = N + 1;
    adj2 = N - size;

    attach_if_full(adj1);
    attach_if_full(adj2);

    return;
  }

  if (N == (size) * (size)-1) {
    adj1 = N - 1;
    adj2 = N - size;

    attach_if_full(adj1);
    attach_if_full(adj2);

    return;
  }

  // Ahora se chequea si esta en la primera o ultima fila;
  if ((ii == 0) && (jj != 0) && (jj != (size - 1))) {
    adj1 = N - 1;
    adj2 = N + 1;
    adj3 = N + size;

    attach_if_full(adj1);
    attach_if_full(adj2);
    attach_if_full(adj3);

    return;
  }

  if ((ii == size - 1) && (jj != 0) && (jj != (size - 1))) {
    adj1 = N - 1;
    adj2 = N + 1;
    adj3 = N - size;

    attach_if_full(adj1);
    attach_if_full(adj2);
    attach_if_full(adj3);

    return;
  }

  // Ahora se chequea si está en la primera o ultima columna

  if ((jj == 0) && (ii != 0) && (ii != (size - 1))) {
    adj1 = N - size;
    adj2 = N + 1;
    adj3 = N + size;

    attach_if_full(adj1);
    attach_if_full(adj2);
    attach_if_full(adj3);

    return;
  }

  if ((jj == (size - 1)) && (ii != 0) && (ii != (size - 1))) {
    adj1 = N - size;
    adj2 = N - 1;
    adj3 = N + size;

    attach_if_full(adj1);
    attach_if_full(adj2);
    attach_if_full(adj3);

    return;
  }

  // Finalmente si no se tiene ninguno de los anteriores casos.
  adj1 = N - size;
  adj2 = N - 1;
  adj3 = N + 1;
  adj4 = N + size;

  attach_if_full(adj1);
  attach_if_full(adj2);
  attach_if_full(adj3);
  attach_if_full(adj4);
}

bool is_percolating(const std::vector<long> &data, const long cluster_num) {
  const long size = std::sqrt(data.size());

  /*Para saber si el cluster numero N es percolante necesito encontrar,
   *despues de haber clasficado, el número N en la primera y ultima fila, o
   *el numero N, en la primera y última columna. Esas son las condiciones a
   *verificar*/
  bool condition1 = false;
  bool condition2 = false;

  long ii = 0;

  // Primero se chequea para las filas.
  for (ii = 0; ii < size; ++ii) {
    if (data[ii] == cluster_num) {
      condition1 = true;
    }
  }

  for (ii = 0; ii < size; ++ii) {
    if (data[size * (size - 1) + ii] == cluster_num) {
      condition2 = true;
    }
  }

  if (condition1 && condition2) {
    return true;
  } else {
    condition1 = false;
    condition2 = false;
  }

  // Ahora se chequea para las columnas.

  for (ii = 0; ii < size; ++ii) {
    if (data[ii * size] == cluster_num) {
      condition1 = true;
    }
  }

  for (ii = 0; ii < size; ++ii) {
    if (data[(size - 1) + ii * size] == cluster_num) {
      condition2 = true;
    }
  }

  if (condition1 && condition2) {
    return true;
  } else {
    return false;
  }
}

bool is_there_a_percolating(const std::vector<long> &data) {
  const long N = number_of_last_cluster(data);

  for (long kk = 2; kk <= N; ++kk) {
    if (is_percolating(data, kk)) {
      return true;
    }
  }

  return false;
}

long largest_percolating_cluster_size(const std::vector<long> &data) {
  if (is_there_a_percolating(data) == false) {
    return 0;
  }

  long largest_size = 0;

  const long N = number_of_last_cluster(data);

  for (long ii = 2; ii <= N; ++ii) {
    long size_of_cluster_ii = cluster_size(data, ii);

    if (size_of_cluster_ii > largest_size) {
      largest_size = size_of_cluster_ii;
    }
  }

  return largest_size;
}

long number_of_last_cluster(const std::vector<long> &data)
{
  const long size = data.size();
  
  long ii = 0;

  for (ii = 2; ii < size; ++ii) {
    if (std::none_of(data.begin(), data.end(),
                     [ii](int k) { return k == ii; })) {
      break;
    }
  }
  
  if (ii <= 2) {
    return 0;
  }

  return (ii - 1);
}

double compute_mean(const std::vector<double> &data) {
  return std::accumulate(data.begin(), data.end(), 0.0) / data.size();
}

double standard_deviation(const std::vector<double> &data, double mean)
{
  const long size = data.size();
  
  double std_dev = 0.0;

  for (int kk = 0; kk < size; ++kk) {
    std_dev += (data[kk] - mean) * (data[kk] - mean);
  }

  std_dev = std::sqrt(std_dev / data.size());

  return std_dev;
}

void compute_mean_and_standard_deviation_for_percolating_probability(
    long size, double probability, int reps_per_single_calculation,
    int number_of_calculations, int seed) {
  
  auto start = std::chrono::steady_clock::now();
  
  std::cout.precision(16);

  // Matriz que se llenará
  std::vector<long> m(size * size, 0);

  std::vector<double> probability_of_percolating(number_of_calculations, 0.0);

  std::mt19937 generator(seed);

  // Este será el generador de las semillas que se necesitan para llenar la
  // matriz.
  std::uniform_int_distribution<int> new_seeds(1, 5000);

  for (int jj = 0; jj < number_of_calculations; ++jj) {
    double one_calculation_prob = 0.0;

    int number_of_percolatings = 0;

    for (int ii = 0; ii < reps_per_single_calculation; ++ii) {
      int fill_seed = new_seeds(generator);

      fill_matrix(m, fill_seed, probability);

      classify_clusters(m);

      if (is_there_a_percolating(m) == true) {
        ++number_of_percolatings;
      }
    }

    one_calculation_prob =
        1.0 * (number_of_percolatings) / (1.0 * reps_per_single_calculation);

    probability_of_percolating[jj] = one_calculation_prob;
  }

  double mean_probability = compute_mean(probability_of_percolating);

  double std_deviation_of_probability =
      standard_deviation(probability_of_percolating, mean_probability);

  std::cout << probability << '\t';

  std::cout << mean_probability << '\t' << std_deviation_of_probability << '\n';

  auto end = std::chrono::steady_clock::now();
  
  std::chrono::duration<double> elapsed_seconds = end-start;

  std::cerr << size << '\t' << probability << '\t' << elapsed_seconds.count() << '\n';

}

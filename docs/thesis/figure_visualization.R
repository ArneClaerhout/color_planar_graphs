install.packages("readxl")
install.packages("latex2exp")
library(readxl)
library(latex2exp)

getwd()
setwd("//wsl.localhost/Ubuntu-22.04/home/arne/color_planar_graphs/docs/thesis")

# First, the data is read
columns_per_version = 12

# This was made using AI
parser = function(data) {
  total_cols <- ncol(data)
  num_sets <- total_cols / columns_per_version
  
  # Initialize an empty list to store the chunks
  all_chunks <- list()
  
  # 3. Loop through the horizontal blocks
  for (i in 1:num_sets) {
    # Calculate start and end column indices for the current block
    start_col <- ((i - 1) * columns_per_version) + 1
    end_col <- i * columns_per_version
    
    # Extract the chunk
    current_chunk <- data[, start_col:end_col]
    
    raw_name <- colnames(current_chunk)[1]
    clean_name <- gsub("[[:space:]./]+", "_", raw_name)
    
    new_headers <- colnames(current_chunk)
    new_headers[1] <- "Number_of_Vertices"
    colnames(current_chunk) <- new_headers
    
    # Store in the list
    all_chunks[[clean_name]] <- as.matrix(current_chunk)
  }
  return(all_chunks)
}

General = parser(read_excel("thesis_data.xlsx", sheet=2))
C = parser(read_excel("thesis_data.xlsx", sheet=3))
Index = parser(read_excel("thesis_data.xlsx", sheet=4))
Bitsets = parser(read_excel("thesis_data.xlsx", sheet=5))
Datastructures = parser(read_excel("thesis_data.xlsx", sheet=6))

# Then, the data can be used to make graphs, also made using AI
plot_data = function(input_data, legend_text, title_text) {
  # Set the global font to Serif
  par(family = "serif", mar = c(6, 6, 4, 2) + 0.05)
  
  # Use a professional color palette (ColorBlind Friendly)
  colors <- c("#264653", "#2a9d8f", "#e9c46a", "#f4a261", "#e76f51")
  
  # Initialize Plot
  plot(1, type = "n", log = "y", 
       xlim = range(sapply(input_data, function(df) df[, 1]), na.rm = TRUE),
       ylim = range(sapply(input_data, function(df) df[, 12]), na.rm = TRUE),
       xlab = TeX("Number of vertices ($n$)"), 
       ylab = TeX("Cumulative time needed ( $ ms$)"), 
       main = title_text,
       axes = FALSE,
       cex.lab = 1.5,   
       cex.axis = 1.2,  
       cex.main = 1.8, 
       mgp = c(3.5, 1, 0),
       las = 1)
  
  # Add the lines with different styles
  for (i in seq_along(input_data)) {
    lines(input_data[[i]][, 1], input_data[[i]][, 12], 
          col = colors[i], lty = i, lwd = 3)
  }
  
  # Add axes and legend
  axis(1); axis(2, las = 1); box()
  legend("topleft", legend = legend_text, col = colors, lty = 1:length(legend_text), lwd = 2, bty = "n", cex = 1.2)
}


# General plot
legend_text = c("Naive", "Optimized", "Second order", "Final", "Multiprocessed")
title_text = "Analysis of implementations"

plot_data(General, legend_text, title_text)

# Bitset plot
legend_text = c("Without bit-set", "With bit-set")
title_text = "Effect of bit-set usage"

plot_data(Bitsets, legend_text, title_text)

# Datastructures plot
legend_text = c("List", "Priority Queue", "Bit-set")
title_text = "Effect of data structure"

plot_data(Datastructures, legend_text, title_text)

# Index plot
legend_text = c("Standard", "WP", "Standard_WP")
title_text = "Effect of index choosing method"

plot_data(Index, legend_text, title_text)

# Pure C effect plot
legend_text = c("Java", "C")
title_text = "Effect of Programming language"

plot_data(C, legend_text, title_text)







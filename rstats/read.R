library(tidyverse)
library(wtl)
loadNamespace("cowplot")

tek_params = c("n", "alpha", "beta", "lambda", "xi", "nu", "lower", "upper", "coexist")
extract_params = function(filename, params=tek_params) {
  patterns = sprintf("_%s([^_]+)_", params)
  str_match(paste0("_", filename), patterns)[, 2] %>%
    parse_double() %>%
    set_names(params) %>%
    as.list() %>%
    as_tibble()
}
# .infiles[1] %>% extract_params()

read_metadata = function(dirs) {
  dirs %>%
    str_subset("_\\d+$") %>%
    set_names() %>%
    purrr::map_dfr(extract_params, .id = "indir") %>%
    dplyr::group_by(!!!rlang::syms(tek_params)) %>%
    dplyr::mutate(repl = seq_len(n())) %>%
    dplyr::ungroup() %>%
    dplyr::arrange(n, xi, coexist, lower, upper, repl)
}

filter_0x0 = function(x) dplyr::filter(x, label == "0x0")

source("~/git/tek2/rstats/activity.R")
source("~/git/tek2/rstats/biostrings.R")
source("~/git/tek2/rstats/treestats.R")

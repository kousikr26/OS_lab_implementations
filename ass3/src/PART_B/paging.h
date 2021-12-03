#define PAGING_H


void handle_page_fault();
int get_swapped_block(pde_t *pgdir, uint va);
void map_physical_to_virtual(pde_t *pgdir, uint addr);
void swap_page_from_pte(pte_t *pte);
int swap_page(pde_t *pgdir);
pte_t* select_victim(pde_t *pgdir);
void clear_valid_bits(pde_t *pgdir);

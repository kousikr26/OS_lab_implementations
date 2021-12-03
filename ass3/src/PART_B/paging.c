#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "x86.h"
#include "traps.h"
#include "spinlock.h"
#include "paging.h"
#include "fs.h"


static pte_t * walk_page_directory(pde_t *pgdir, const void *va, int alloc);
int deallocuvm_new(pde_t *pgdir, uint oldsz, uint newsz);
static int mappages(pde_t *pgdir, void *va, uint size, uint pa, int perm);

struct proc*
myproc_new(void) {
  struct proc *p;
  pushcli();
 
  p = mycpu()->proc;
  
  popcli();
  return p;
}

// Allocate eight consecutive disk blocks.
// Write the dirty page in the physical memory to disk and update PTE.
void swap_page_from_pte(pte_t *pte)
{
  
  //PTE_ADDR returns address in pte
  uint phy_addr = PTE_ADDR(*pte);          
  uint disk_pg = balloc_page(ROOTDEV);

  if(!phy_addr)
  {
    cprintf("phy_addr address is zero\n");
  }
  
  // write disk_pg to disk
  write_page_to_disk(ROOTDEV, (char*)P2V(phy_addr), disk_pg);    // defined in bio.c

  // update the PTE corresponding to the swapped page (block id and Swapped flag)
  
  //make pte = null;
  *pte = (*pte & 0x000000);     
  *pte = (disk_pg << 12)| PTE_SWAPPED;
  *pte = *pte & ~PTE_P;

  kfree(P2V(phy_addr));
  cprintf("\nReturning from swap page from PTE\n");

}

// Swap page after selecting a victim frame
int swap_page(pde_t *pgdir)
{
  pte_t* pte = select_victim(pgdir);            // defined in vm.c

  // Victim not found in first attempt. 
  if(!pte){                                     
    
    cprintf("\nVICTIM NOT FOUND in first attempt.\n Clearing valid BITS.\n");
    
    // Clear valid bits
    clear_valid_bits(pgdir);                    // defined in vm.c    

    cprintf("\nFINDING VICTIM AGAIN...\n");
    
    // Select a victim again
    pte = select_victim(pgdir);                   

    if(pte) 
    {
      cprintf("\nVICTIM FOUND\n");
    }
    else cprintf("\nVICTIM NOT FOUND in second attempt.\n" );
  }
  else{                                          
    cprintf("\nVICTIM FOUND in first attempt.\n");
  }

  // swap victim page to disk
  swap_page_from_pte(pte);  

  // Flush the older TLB entries
  lcr3(V2P(pgdir));         

	return 1;
}

/* Map a physical page to the virtual address addr.
 * If the page table entry points to a swapped block
 * restore the content of the page from the swapped
 * block and free the swapped block.
 */
void map_physical_to_virtual(pde_t *pgdir, uint addr)
{

  struct proc *curr_proc = myproc_new();

	uint cursz = curr_proc->sz;

  // rounds the address to a multiple of page size (PGSIZE)
	uint a = PGROUNDDOWN(rcr2());			

  pte_t *pte = walk_page_directory(pgdir, (char*)a, 0);

  // disk block id where the page was swapped
  int blockid = -1;                 

  // allocate a phy page
	char *mem = kalloc();    

  if(!mem){
	
    swap_page(pgdir);
    mem=kalloc();             
    cprintf("kalloc success\n");

	}

  if(pte!=0){

    if(*pte & PTE_SWAPPED){
      cprintf("inn");
      //disk block id where the page was swapped
      blockid = get_swapped_block(pgdir,a);      

      read_page_from_disk(ROOTDEV, mem, blockid);

      *pte = V2P(mem) | PTE_W | PTE_U | PTE_P;
      *pte &= ~PTE_SWAPPED;
      lcr3(V2P(pgdir));
      bfree_page(ROOTDEV,blockid);
    }

    else{
      memset(mem,0,PGSIZE);
    
    	if(mappages(pgdir, (char*)a, PGSIZE, V2P(mem), PTE_P | PTE_W | PTE_U )<0){
    		panic("\nallocuvm OUT OF MEMORY\n");
    		deallocuvm_new(pgdir,cursz+PGSIZE, cursz);
    		kfree(mem);
    	}
    	else{
        ;
    		// cprintf("Swapping\n");
    	}
    }
  }
  // panic("map_physical_to_virtual is not implemented");
}

/* page fault handler */
void
handle_page_fault()
{
  // cprintf("Page fault\n");
	unsigned addr;
	struct proc *curr_proc = myproc_new();

	asm volatile ("movl %%cr2, %0 \n\t" : "=r" (addr));
	addr &= ~0xfff;

	map_physical_to_virtual(curr_proc->pgdir, addr);
}


// Return the address of the PTE in page table pgdir that corresponds to virtual address va.  
// If alloc!=0, create any required page table pages.
static pte_t * walk_page_directory(pde_t *pgdir, const void *va, int alloc)
{

  pde_t *pde;
  pte_t *pgtab;

  pde = &pgdir[PDX(va)];
  if(*pde & PTE_P)
  {
    pgtab = (pte_t*)P2V(PTE_ADDR(*pde));
  }
  else
  {
    if(!alloc || (pgtab = (pte_t*)kalloc()) == 0)
    {
      return 0;
    }

    // Make sure all those PTE_P bits are zero.
    memset(pgtab, 0, PGSIZE);

    // updating permissions
    *pde = V2P(pgtab) | PTE_P | PTE_W | PTE_U;
  }

  return &pgtab[PTX(va)];
}

// Deallocate user pages to bring the process size from oldsz to newsz. Returns the new process size.
// If the page was swapped free the corresponding disk block.

int deallocuvm_new(pde_t *pgdir, uint oldsz, uint newsz)
{
  pte_t *pte;
  uint a, pa;

  if(newsz >= oldsz)
  {
    return oldsz;
  }

  a = PGROUNDUP(newsz);
  while(a<oldsz)
  {
    pte = walk_page_directory(pgdir, (char*)a, 0);
    if(!pte)
    {
      a = PGADDR(PDX(a) + 1, 0, 0) - PGSIZE;
    }
    else if((*pte & PTE_P))
    {
      pa = PTE_ADDR(*pte);
      if(!pa)
      {
        panic("kfree");
      }

      char *v = P2V(pa);
      kfree(v);
      *pte = 0;
    }

    a += PGSIZE;
  }
  return newsz;
}


// Create PTEs for virtual addresses starting at va that refer to physical addresses starting at pa.
static int mappages(pde_t *pgdir, void *va, uint size, uint pa, int perm)
{
  char *a, *last;
  pte_t *pte;

  a = (char*)PGROUNDDOWN((uint)va);
  last = (char*)PGROUNDDOWN(((uint)va) + size - 1);
  
  while(1){
    if((pte = walk_page_directory(pgdir, a, 1)) == 0)
    {
      return -1;
    }

    *pte = pa | perm | PTE_P;
    
    if(a == last)
    {
      break;
    }
    a += PGSIZE;
    pa += PGSIZE;
  }
  return 0;
}

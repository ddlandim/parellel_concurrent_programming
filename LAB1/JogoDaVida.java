package pcd;

import java.util.Random;
import java.util.concurrent.BrokenBarrierException;
import java.util.concurrent.CyclicBarrier;

public class JogoDaVida {
	
	final static int NUM_THREADS = 2;
	final static int DEAD = 0;
	final static int ALIVE = 1;
	final static int SIZE = 2048;
	static CyclicBarrier barr;
	
	static int grid[][] = new int[SIZE][SIZE];
	static int newGrid[][] = new int[SIZE][SIZE];
	
	static class Exec implements Runnable {
		private int inicio;
		private int fim;
		private int count;
		
		public Exec(int ini, int fim, int count) {
			super();
			this.inicio = ini;
			this.fim = fim;
			this.count = count;
			
		}

		@Override
		public void run() {
			int i, g;
			
			
			for(g = 0; g < 2000; g++) {
			
			for (i = inicio; i < fim; i++){
				for (int j = 0; j < SIZE; j++){
					/* living cells with less than 2 neighbors die of abandonment 
					living cells with 4 or more neighbors die of overpopulation */
					if(grid[i][j] == ALIVE){
						if(JogoDaVida.getNeighbors(grid, i, j) < 2) {
							newGrid[i][j] = DEAD;
							this.count--;
						} else if(JogoDaVida.getNeighbors(grid, i, j) >= 4){
							newGrid[i][j] = DEAD;
							this.count--;
						}else if(JogoDaVida.getNeighbors(grid, i, j) == 2 || JogoDaVida.getNeighbors(grid, i, j) == 3) {
							newGrid[i][j] = ALIVE;
						}
					} else {
						if(JogoDaVida.getNeighbors(grid, i, j) == 3){
							newGrid[i][j] = ALIVE;
							this.count++;
			
						} 
					}  
				}
			}
			try {
				barr.await();
			} catch (InterruptedException | BrokenBarrierException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			
			

			for (i = inicio; i < fim; i++){
				for (int j = 0; j < SIZE; j++){
					grid[i][j] = newGrid[i][j];
				}
			}
			try {
				barr.await();
			} catch (InterruptedException | BrokenBarrierException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			

		
			
		
			
		}
	}
		public int getCount() {
			return this.count;
		}
		
		public int getInicio() {
			return this.inicio;
		}
		public int getFim() {
			return this.fim;
		}
		
	}
	
	
	
	static public void startGeneration(int grid[][]) {
		Random gerador = new Random(1985);
		for(int i = 0; i<2048; i++) { 
			   for(int j = 0; j<2048; j++) {
			      grid[i][j] = gerador.nextInt(2147483647) % 2;
			   }
			}
		
	}
	
	static public int getNeighbors(int grid[][], int i, int j) {
		int count = 0;
		/*----TOP ROW----*/
	    if(i == 0) {

	    	if(grid[SIZE-1][j] == ALIVE) count++; //upstairs neighbor
			if(grid[i+1][j] == ALIVE) count++; //downstairs neighbor

			//top left corner 
			if(j == 0){
				//left neighbors
				if(grid[SIZE-1][SIZE-1] == ALIVE) count++;
				if(grid[i][SIZE-1] == ALIVE) count++;
				if(grid[i+1][SIZE-1] == ALIVE) count++;

				//right neighbors
				if(grid[SIZE-1][j+1] == ALIVE) count++;
				if(grid[i][j+1] == ALIVE) count++;
				if(grid[i+1][j+1] == ALIVE) count++;

				return count;
			}

			//top right corner
			if(j == SIZE - 1){
				//left neighbors
				if(grid[SIZE-1][j-1] == ALIVE) count++;
				if(grid[i][j-1] == ALIVE) count++;
				if(grid[i+1][j-1] == ALIVE) count++;
				
				//right neighbors
				if(grid[SIZE-1][0] == ALIVE) count++;
				if(grid[i][0] == ALIVE) count++;
				if(grid[i+1][0] == ALIVE) count++;
				
				return count;
			}

			//not in the corners
			else {


				//left neighbors
				if(grid[SIZE-1][j-1] == ALIVE) count++;
				if(grid[i][j-1] == ALIVE) count++;
				if(grid[i+1][j-1] == ALIVE) count++;
				
				//right neighbors
				if(grid[SIZE-1][j+1] == ALIVE) count++;
				if(grid[i][j+1] == ALIVE) count++;
				if(grid[i+1][j+1] == ALIVE) count++;

				return count;
			}
		}

		/*/*----BOTTOM ROW----*/
		if(i == SIZE - 1) {

			if(grid[i-1][j] == ALIVE) count++; //upstairs neighbor
			if(grid[0][j] == ALIVE) count++; //downstairs neighbor

			//bottom left corner 
			if(j == 0){
				//left neighbors
				if(grid[i-1][SIZE-1] == ALIVE) count++;
				if(grid[i][SIZE-1] == ALIVE) count++;
				if(grid[0][SIZE-1] == ALIVE) count++;
				

				//right neighbors
				if(grid[i-1][j+1] == ALIVE) count++;
				if(grid[i][j+1] == ALIVE) count++;
				if(grid[0][j+1] == ALIVE) count++;
				return count;
			}

			//bottom right corner
			if(j == SIZE - 1){

				//left neighbors
				if(grid[i-1][j-1] == ALIVE) count++;
				if(grid[i][j-1] == ALIVE) count++;
				if(grid[0][j-1] == ALIVE) count++;
				
				//right neighbors
				if(grid[i-1][0] == ALIVE) count++;
				if(grid[i][0] == ALIVE) count++;
				if(grid[0][0] == ALIVE) count++;
				
				
				return count;
			}

			//not in the corners
			else {

				//left neighbors
				if(grid[i-1][j-1] == ALIVE) count++;
				if(grid[i][j-1] == ALIVE) count++;
				if(grid[0][j-1] == ALIVE) count++;
				
				//right neighbors
				if(grid[i-1][j+1] == ALIVE) count++;
				if(grid[i][j+1] == ALIVE) count++;
				if(grid[0][j+1] == ALIVE) count++;

				return count;
			}
		}

		if(grid[i-1][j] == ALIVE) count++; //upstairs neighbor
	    if(grid[i+1][j] == ALIVE) count++; //downstairs neighbor

		/*----FIRST COLUMN----*/
		if(j == 0) {
			
			//left neighbors
			if(grid[i-1][SIZE-1] == ALIVE) count++;
			if(grid[i][SIZE-1] == ALIVE) count++;
			if(grid[i+1][SIZE-1] == ALIVE) count++;

			//right neighbors
			if(grid[i-1][j+1] == ALIVE) count++;
			if(grid[i][j+1] == ALIVE) count++;
			if(grid[i+1][j+1] == ALIVE) count++;

			return count;
		} 
		
		/*----LAST COLUMN----*/
		if(j == SIZE - 1) {
			

			//left neighbors
			if(grid[i-1][j-1] == ALIVE) count++;
			if(grid[i][j-1] == ALIVE) count++;
			if(grid[i+1][j-1] == ALIVE) count++;

			//right neighbors
			if(grid[i-1][0] == ALIVE) count++;
			if(grid[i][0] == ALIVE) count++;
			if(grid[i+1][0] == ALIVE) count++;

			return count;

		}

		/*----NOT IN THE BORDERS----*/

		//left neighbors
		if(grid[i-1][j-1] == ALIVE) count++;
		if(grid[i][j-1] == ALIVE) count++;
		if(grid[i+1][j-1] == ALIVE) count++;

		//right neighbors
		if(grid[i-1][j+1] == ALIVE) count++;
		if(grid[i][j+1] == ALIVE) count++;
		if(grid[i+1][j+1] == ALIVE) count++;

		return count;
	}
	
	
	

	static public void main(String Args[]) {
		
		long startTime = System.currentTimeMillis();
		
		Exec[] ex = new Exec[NUM_THREADS];
		Thread[] ti = new Thread[NUM_THREADS];		
		JogoDaVida.startGeneration(grid);
		barr= new CyclicBarrier(NUM_THREADS); 
		int ratio = SIZE/NUM_THREADS;
		int i;
		
		long lacoStartTime = System.currentTimeMillis();
		
		int total = 0;
		for (i = 0; i < SIZE; i++){
			for (int j = 0; j < SIZE; j++)
				if(grid[i][j] == ALIVE) total++;
		}
		
		for(i = 0; i < NUM_THREADS; i++) {
			ex[i] = new Exec(i*ratio, (i*ratio) + ratio, total/NUM_THREADS);
			ti[i] = new Thread(ex[i]);
			ti[i].start();
		}
		System.out.println("Geração 0: " +total);
		
		try {
			ti[0].join();
            for(i = 1; i < NUM_THREADS; i++) {
                ti[i].join();
              
            }
        } catch (InterruptedException e) {
        	System.out.println("Exceotion");
        }
		long lacoFinal = System.currentTimeMillis() - lacoStartTime;
		
		total = 0;
		for(i = 0; i < NUM_THREADS; i++) {
			total += ex[i].getCount();	
		}
		
		System.out.println("Total: " +total);
		
		long calcTime = System.currentTimeMillis() - startTime;
		System.out.println("Tempo laco: " +lacoFinal);
		System.out.println("Tempo total: " +calcTime);

		
	}

}

/* Geração 0: 2097152
Geração0:1133104
Geração1:1087296
Geração2:1061328
Total: 1061328 */
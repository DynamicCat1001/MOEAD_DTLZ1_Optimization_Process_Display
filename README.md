# **Overview**
This project is used to demonstrate the optimization process of multi-objective evolution algorithm on the test function DTLZ1.

# **requirement**
> * The glut and eigen library are used to support the User Interface and the matrix evaluation respectively. 
> * These two libraries might be installed and linked well before operating this project.
# **flow**
* **Input iteration value**
> * Input maximum iteration of optimization between range 10~1000.
* **Read reference points**
> * Read best solution data for IGD indicator calculation.
* **Optimizing**
> * Execute MOEAD on test function DTLZ1. 
> * The result of every iteration are stored.
> * Calculate IGD
* **Display the results**
> * Rendering with the stored solution Group of different iteration
> * Show the final solution and IGD indicator.

# **key feature**
> * Display interface built by glut library.
>> * optimization process among iterations divided by every 20 iterations.
>> * distribution and IGD value of final elite solution group of max iteration.
> * optimization of MOEAD on test function DTLZ1.
# **Step**
> 1. input max iteration within the range of 10~1000.
![image](https://github.com/user-attachments/assets/348cf57a-7735-4afa-b32b-8ff55469c12f)
> 2. After the optimization, the Interface shows the results.
![2024-12-02_23-12-52](https://github.com/user-attachments/assets/d8af2c74-d4b9-4442-bff5-ee28eac9efeb)

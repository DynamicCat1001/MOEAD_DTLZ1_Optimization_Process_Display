# **Overview**
This project demonstrates the optimization process of the multi-objective evolution algorithm on the test function DTLZ1.

# **requirement**
> * The glut and eigen library support the User Interface and the matrix operation respectively. 
> * These two libraries should be installed and linked well to operate this project.
# **flow of project**
* **get iteration value**
> * get maximum iteration of optimization between range 10~1000 from user input.
* **Read reference points**
> * Read the best solution data to calculate IGD indicator to estimate the performance of MOEAD on test problems.
* **Optimizing**
> * Execute optimization of MOEAD on test function DTLZ1.
> * The results of every iteration are stored.
> * Calculate IGD
* **Display the results**
> * construct the objective space animation with the stored solution Group of iterations.
> * Construct the object of the final solution group and show the IGD indicator.

# **key feature**
> * Display interface constructed by glut library.
>> * The optimization process among iterations divided by every 20 iterations is displayed on the left window.
>> * The distribution and IGD value of the final elite solution group of max iteration is displayed on the right window.
> * The optimization of MOEAD on test function DTLZ1.
# **Step**
> 1. input max iteration within the range of 10~1000.
![image](https://github.com/user-attachments/assets/348cf57a-7735-4afa-b32b-8ff55469c12f)
> 2. After the optimization, the Interface shows the results.
> 3. Drag the interface to see the distribution in the objective space.
![2024-12-02_23-12-52](https://github.com/user-attachments/assets/d8af2c74-d4b9-4442-bff5-ee28eac9efeb)

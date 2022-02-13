/*
 * Project2_FCE_TaskScheduling_MCC.cpp
 *
 *  Created on: Dec 2, 2021
 *      Author: snehal
 */



#include <iostream>
#include <chrono>
#include <stack>
#include <vector>
#include <algorithm>
#include <list>
#include <iterator>
#include <iomanip>
using namespace std;

//Initial Scheduling - Phase one
void primary_assignment(vector<vector<int> > time_task_core_array,int cloud[], int Ti_cloud, int n, int k){
    int min[n];
    for(int i=0; i<n; i++){
        min[i] = INT_MAX;
    }
    for(int i=0; i<n; i++){
        for(int j=0; j<k; j++){
            if(min[i]>time_task_core_array[i][j]){
                min[i] = time_task_core_array[i][j];
            }
        }
    }
    for(int i=0; i<n; i++){
        if(min[i] > Ti_cloud){
            cloud[i] = 1;
        }
        else
            cloud[i] = 0;
    }
}

//Initial Scheduling - Phase two
void task_prioritzing(vector<vector<int> > time_task_core_array, int priority[], int priority_sorted[], vector<vector<int> > edge_matrix, int computation_cost[], int cloud[], int Ti_cloud, int n, int k){
    for(int i=0; i<n; i++){
        if(cloud[i] == 1){
        	computation_cost[i] = Ti_cloud;
        }
        else{
            int sum = 0;
            for(int j=0; j<k; j++){
                sum += time_task_core_array[i][j];
            }
            computation_cost[i] = sum/k;
        }
    }
    priority[n-1] = computation_cost[n-1];
    for(int i=n-1; i>=0; i--){
        int max_j = 0;
        for(int j=n-1; j>=0; j--){
            if(edge_matrix[i][j] == 1 && priority[j] > max_j){
                max_j = priority[j];
            }
        }
        priority[i] = computation_cost[i] + max_j;
    }
    //calculating and sorting priority
    vector<pair<int,int> > vect;
    for (int i=0; i<n; i++){
        vect.push_back(make_pair(priority[i],i));
    }
    sort(vect.begin(), vect.end());
    for(int i=0; i<n; i++){
    	priority_sorted[i] = vect[i].second;
    }
}

//Initial Scheduling - Phase three
void execution_unit_selection(vector<vector<int> > time_task_core_array,int priority_sorted[], vector<vector<int> > edge_matrix, int cloud[], int RT_l[], int RT_c[], int RT_ws[], int FT_ws[], int FT_wr[], int FT_l[], int finish_time[], int core[], int core1[], int n, int k, int Ts, int Tr, int Tc, vector<vector<int> > scheduled_array){
    int first_priority_index = priority_sorted[n-1];//scheduling first priority task
    RT_l[first_priority_index] = 0;
    RT_ws[first_priority_index] = 0;
    FT_ws[first_priority_index] = RT_ws[first_priority_index] + Ts;
    RT_c[first_priority_index] = FT_ws[first_priority_index];
    if(cloud[first_priority_index] == 1){
        FT_wr[first_priority_index] = RT_c[first_priority_index] + Tc + Tr;
        FT_l[first_priority_index] = 0;
        finish_time[first_priority_index] = FT_wr[first_priority_index];
        core[3] = finish_time[first_priority_index];
        scheduled_array[0].push_back(first_priority_index);
        core1[first_priority_index] = 3;
    }
    else{
        int min = INT_MAX;
        int index;
        for(int i=0; i<k; i++){
            if(time_task_core_array[first_priority_index][i]<min){
                min = time_task_core_array[first_priority_index][i];
                index = i;
            }
        }
        FT_l[first_priority_index] = RT_l[first_priority_index] + min;
        FT_wr[first_priority_index] = RT_c[first_priority_index] + Tc + Tr;
        if(FT_l[first_priority_index] <= FT_wr[first_priority_index]){
            finish_time[first_priority_index] = FT_l[first_priority_index];
            FT_wr[first_priority_index] = 0;
            core[3] = FT_ws[first_priority_index];
            scheduled_array[index+1].push_back(first_priority_index);
            core1[first_priority_index] = index;
        }
        else{
            finish_time[first_priority_index] = FT_wr[first_priority_index];
            FT_l[first_priority_index] = 0;
            core[index] = finish_time[first_priority_index];
            scheduled_array[0].push_back(first_priority_index);
            core1[first_priority_index] = 3;
        }
    }
    //scheduling rest of the elements
    for(int a=n-2; a>=0; a--){
        int i = priority_sorted[a];
        int max_j_l = 0;
        for(int j=0; j<n; j++){
            if(edge_matrix[j][i] == 1 && max_j_l < max(FT_l[j],FT_wr[j])){
                max_j_l = max(FT_l[j],FT_wr[j]);
            }
        }
        RT_l[i] = max_j_l;
        int max_j_ws = 0;
        for(int j=0; j<n; j++){
            if(edge_matrix[j][i] == 1 && max_j_ws < max(FT_l[j],FT_ws[j])){
                max_j_ws = max(FT_l[j],FT_ws[j]);
            }
        }
        RT_ws[i] = max_j_ws;
        FT_ws[i] = max(core[3],RT_ws[i]) + Ts;
        int max_j_c = 0;
        for(int j=0; j<n; j++){
            if(edge_matrix[j][i] == 1 && max_j_c < FT_wr[j]-Tr){
                max_j_c = FT_wr[j]-Tr;
            }
        }
        RT_c[i] = max(FT_ws[i],max_j_c);
        if(cloud[i] == 1){
            FT_wr[i] = RT_c[i] + Tc + Tr;
            finish_time[i] = FT_wr[i];
            FT_l[i] = 0;
            core[3] = FT_ws[i];
            scheduled_array[0].push_back(i);
            core1[i] = 3;
        }
        else{
            int rt, index;
            int f = INT_MAX;
            for(int j=0; j<k; j++){
                rt = max(RT_l[i],core[j]);
                if(f > rt + time_task_core_array[i][j]){
                    f = rt + time_task_core_array[i][j];
                    index = j;
                }
            }
            RT_l[i] = f - time_task_core_array[i][index];
            FT_l[i] = f;
            FT_wr[i] = RT_c[i] + Tc + Tr;
            if(FT_l[i] <= FT_wr[i]){
                finish_time[i] = FT_l[i];
                FT_wr[i] = 0;
                core[index] = finish_time[i];
                scheduled_array[index+1].push_back(i);
                core1[i] = index;
            }
            else{
                finish_time[i] = FT_wr[i];
                FT_l[i] = 0;
                core[3] = finish_time[i];
                scheduled_array[0].push_back(i);
                core1[i] = 3;
            }
        }
    }
}

//Task Migration - outer loop + Kernel algorithm
int task_migration( vector<vector<int> > scheduled_array, vector<vector<int> > time_task_core_array, vector<vector<int> > edge_matrix,int core1[], int tmax, int t_total, float E_total, int n, int k, int start_time[], int end_time[], int E_c, int E_l[][3]){
    int out = 0;
    int count = 0;
    while(out == 0){
        float max_ratio = 0;
        int new_n = 0, new_k = 0, new_index1 = 0, new_index2 = 0, new_t = t_total;
        float new_e = E_total;
        int less_t1 =0, less_t2 = 0;
        int temp_core[n], new_st[n], new_ft[n];;
        for(int i=0; i<n; i++){
            for(int j=0; j<k+1; j++){
                int core2[n], core3[4], rt[n], rt1[n], finish_time[n], ft1[n], pushed[n];
                vector<vector<int> > tlist(4);
                int index1, index2 = 0;
                for(int i=0; i<n; i++){
                    rt[i] = 0;
                    finish_time[i] = 0;
                    core2[i] = core1[i];
                    ft1[i] = end_time[i];
                    rt1[i] = start_time[i];
                    pushed[i] = 0;
                }
                for(int a=0; a<scheduled_array.size(); a++){
                    core3[a] = 0;
                    for(int b=0; b<scheduled_array[a].size(); b++){
                        tlist[a].push_back(scheduled_array[a][b]);
                    }
                }
                int current_core = core1[i];
                for(int a=0; a<tlist[current_core].size(); a++){
                    if(tlist[current_core][a] == i){
                        index1 = a;
                    }
                }
                tlist[current_core].erase(tlist[current_core].begin()+index1);
                //calculating the ready time of target task
                if(j == 3){
                    int max_j_ws = 0;
                    for(int a=0; a<n; a++){
                        if(edge_matrix[a][i] == 1 && max_j_ws < ft1[a]){
                            max_j_ws = ft1[a];
                        }
                    }
                    rt[i] = max_j_ws;
                }
                else{
                    int max_j_l = 0;
                    for(int a=0; a<n; a++){
                        if(edge_matrix[a][i] == 1 && max_j_l < ft1[a]){
                            max_j_l = ft1[a];
                        }
                    }
                    rt[i] = max_j_l;
                }
                core2[i] = j;

                if(tlist[j].size() == 0){
                    index2 = 0;
                }
                else if(tlist[j].size() == 1){
                    if(rt1[tlist[j][0]] > rt[i]){
                        index2 = 0;
                    }
                    else{
                        index2 = 1;
                    }
                }
                else{
                    if(rt1[tlist[j][0]] > rt[i]){
                        index2 = 0;
                    }
                    else if(rt1[tlist[j][tlist[j].size()-1]] <= rt[i]){
                        index2 = tlist[j].size();
                    }
                    else{
                        for(int b=0; b<tlist[j].size()-1; b++){
                            if(rt[i]>=rt1[tlist[j][b]] && rt[i]<=rt1[tlist[j][b+1]]){
                                index2 = b+1;
                            }
                        }
                    }
                }
                tlist[j].insert(tlist[j].begin()+index2,i);


                int ready1[n], ready2[n];
                for(int a=0; a<n; a++){
                    ready1[a] = 0;
                }
                for(int a=0; a<n; a++){
                    for(int b=0; b<n; b++){
                        if(edge_matrix[a][b] == 1){
                            ready1[b] += 1;
                        }
                    }
                    ready2[a] = 1;
                }

                for(int a=0; a<4; a++){
                    if(tlist[a].size()>0){
                        ready2[tlist[a][0]] = 0;
                    }
                }


                //initialize the stack and implement the first operation
                stack<int> s;
                for(int a=0; a<n; a++){
                    if(ready1[a] == 0 && ready2[a] == 0 && pushed[a] == 0){
                        s.push(a);
                        pushed[a] = 1;

                    }
                }

                int current1 = s.top();
                s.pop();
                rt[current1] = 0;
                if(core2[current1] == 3){
                    rt[current1] = max(core3[core2[current1]],rt[current1]);
                    finish_time[current1] = rt[current1] + 5;
                    core3[core2[current1]] = rt[current1] + 3;
                }
                else{
                    rt[current1] = max(core3[core2[current1]],rt[current1]);
                    finish_time[current1] = rt[current1] + time_task_core_array[current1][core2[current1]];
                    core3[core2[current1]] = finish_time[current1];
                }

                for(int a=0; a<n; a++){
                    if(edge_matrix[current1][a] == 1){
                        ready1[a] -= 1;
                    }
                }
                ready2[current1] = 1;

                if(tlist[core2[current1]].size()>1){
                    for(int a=1; a<tlist[core2[current1]].size(); a++){
                        if(tlist[core2[current1]][a-1] == current1){
                            ready2[tlist[core2[current1]][a]] = 0;

                        }
                    }
                }

                for(int a=0; a<n; a++){
                    if(ready1[a] == 0 && ready2[a] == 0 && pushed[a] == 0){
                        s.push(a);
                        pushed[a] = 1;

                    }
                }

                while(s.size() != 0){
                    int current = s.top();
                    s.pop();

                    //caculate ready time of current task
                    if(core2[current] == 3){
                        int max_j_ws1 = 0;
                        for(int a=0; a<n; a++){
                            if(edge_matrix[a][current] == 1 && max_j_ws1 < finish_time[a]){
                                max_j_ws1 = finish_time[a];
                            }
                        }
                        rt[current] = max_j_ws1;
                    }
                    else{
                        int max_j_l1 = 0;
                        for(int a=0; a<n; a++){
                            if(edge_matrix[a][current] == 1 && max_j_l1 < finish_time[a]){
                                max_j_l1 = finish_time[a];
                            }
                        }
                        rt[current] = max_j_l1;
                    }
                    if(core2[current] == 3){
                        rt[current] = max(core3[core2[current]],rt[current]);
                        finish_time[current] = rt[current] + 5;
                        core3[core2[current]] = rt[current] + 3;
                    }
                    else{
                        rt[current] = max(core3[core2[current]],rt[current]);
                        finish_time[current] = rt[current] + time_task_core_array[current][core2[current]];
                        core3[core2[current]] = finish_time[current];
                    }
                    //update ready1 and ready2
                    for(int a=0; a<n; a++){
                        if(edge_matrix[current][a] == 1){
                            ready1[a] -= 1;
                        }
                    }
                    ready2[current] = 1;
                    if(tlist[core2[current]].size()>1){
                        for(int a=1; a<tlist[core2[current]].size(); a++){
                            if(tlist[core2[current]][a-1] == current){
                                ready2[tlist[core2[current]][a]] = 0;
                            }
                        }
                    }
                    for(int a=0; a<n; a++){
                        if(ready1[a] == 0 && ready2[a] == 0 && pushed[a] == 0){
                            s.push(a);
                            pushed[a] = 1;
                        }
                    }
                }
                int current_t = finish_time[n-1];
                int current_e = 0;
                for(int a=0; a<n; a++){
                    if(core2[a] == 3){
                        current_e += E_c;
                    }
                    else{
                        current_e += E_l[a][core2[a]];
                    }
                }

                if(current_t <= t_total && current_e < new_e){
                    less_t1 = 1;
                    new_n = i;
                    new_k = j;
                    new_index1 = index1;
                    new_index2 = index2;
                    new_t = current_t;
                    new_e = current_e;

                    for(int a=0; a<n; a++){
                    	temp_core[a] = core2[a];
                        new_st[a] = rt[a];
                        new_ft[a] = finish_time[a];
                     }
                }
                if(current_t > t_total && current_t <= tmax && less_t1 == 0 && current_e < E_total && max_ratio < double((E_total - current_e) / (current_t - t_total))){
                    max_ratio = double((E_total - current_e) / (current_t - t_total));
                    //cout<<max_ratio<<endl;
                    less_t2 = 1;
                    new_n = i;
                    new_k = j;
                    new_index1 = index1;
                    new_index2 = index2;
                    new_t = current_t;
                    new_e = current_e;
                    //cout<<new_t<<endl;
                    for(int a=0; a<n; a++){
                        temp_core[a] = core2[a];
                        new_st[a] = rt[a];
                        new_ft[a] = finish_time[a];
                    }
                }
            }
        }
        if(less_t1 != 1 && less_t2 != 1){
            out = 1;
        }
        else{
        	scheduled_array[core1[new_n]].erase(scheduled_array[core1[new_n]].begin()+new_index1);
        	scheduled_array[new_k].insert(scheduled_array[new_k].begin()+new_index2,new_n);
            t_total = new_t;
            E_total = new_e;
            for(int a=0; a<n; a++){
                core1[a] = temp_core[a];
                start_time[a] = new_st[a];
                end_time[a] = new_ft[a];
            }
            if(less_t1 != 1 && less_t2 != 1){
                out = 1;
            }
            count += 1;
            if (count == 1){
            	cout<<"-------Task Migration operations--------"<<endl;
            }
            cout<<"Iteration "<<count<<endl;
            if(new_k+1 > 3)
            {
            	cout<<"Migration of Task "<<new_n+1<<" to Cloud"<<endl;
            }
            else
            {
            	cout<<"Migration of Task "<<new_n+1<<" to Core "<<new_k+1<<endl;
            }
            cout<<"Completion Time: "<<t_total<< "    Energy Consumption: "<<E_total<<endl<<endl;
        }
    }
    cout<<endl;


    cout<<"------Task scheduling result by the MCC Task Scheduling Algorithm------"<<endl;

    for(int i=0; i<scheduled_array.size(); i++){
        if(i == 3){
            cout<<"Cloud: ";
        }
        else{
            cout<<"Core"<<i+1<<": ";
        }
        for(int j=0; j<scheduled_array[i].size(); j++){
            cout<<scheduled_array[i][j]+1<<" ";
        }
        cout<<endl;
    }
    cout<<endl;

    cout<<"Time Assignment Details"<<endl;
    for(int i=0; i<scheduled_array.size(); i++){
        if(i == 3){
            cout<<"Cloud: "<<endl;
        }
        else{
            cout<<"Core "<<i+1<<": "<<endl;
        }
        for(int j=0; j<scheduled_array[i].size(); j++){
            cout<<"      |--Task - "<<scheduled_array[i][j]+1<<" [start time: "<<start_time[scheduled_array[i][j]]<<" finish time: "<<end_time[scheduled_array[i][j]]<<" ]"<<endl;
        }
        cout<<endl;
    }
    cout<<"Energy Consumption: "<<E_total<<"   Completion Time: "<<t_total<<endl;
}

int main(int argc, char *argv[])
{
	int n; // number of task
	int k; // number of cores
	int edge;

	//--------------------------------------------------------------
    //---------	Taking all the required inputs ---------------------------
	//--------------------------------------------------------------
	cout<<"Number of Tasks:"<<endl;
	cin>>n;

	cout<<"Number of Cores:"<<endl;
	cin>>k;

	vector<vector <int> > time_task_core_array(n);
	for (int i = 0; i < n; i++)
	{
		time_task_core_array[i].resize(k);
	}

	cout << "Please give your Task-Core time consumption matrix:" << endl;
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < k; j++)
		{
			cin >> time_task_core_array[i][j];
		}
	}

	cout << "Enter number of edges(links between nodes) in the task order graph: ";
	cin >> edge;

	vector<vector<int> > edge_matrix(n);
	for (int i = 0; i < n; i++)
		{
		edge_matrix[i].resize(n);
		}

	vector<vector<int> > edge_connection(edge);
	for (int i = 0; i < edge; i++)
		{
		edge_connection[i].resize(2);
		}

	cout << "Please input task graph as a matrix where, each row as edge(links between nodes) from node(column 1) to node(column2)" << endl;

	for (int i = 0; i < edge; i++)
	{
			for (int j = 0; j < 2; j++)
			{
				cin >> edge_connection[i][j];
				edge_matrix[edge_connection[i][0]-1][edge_connection[i][1]-1]=1;
			}
	}

    int cloud[n]; //assigning 0 if local else 1 for cloud task
    int priority[n], priority_sorted[n], computation_cost[n], core[4], core1[n];
    int RT_l[n], RT_c[n], RT_ws[n], FT_ws[n], FT_wr[n], FT_l[n], finish_time[n];
    vector<vector<int> > scheduled_array(4);
    for(int i=0; i<n; i++){
        cloud[i] = 0;
        priority[i] = 0;
        priority_sorted[i] = 0;
        computation_cost[i] = 0;
        RT_l[i] = 0;
        RT_ws[i] = 0;
        RT_c[i] = 0;
        FT_ws[i] = 0;
        FT_wr[i] = 0;
        FT_l[i] = 0;
        finish_time[i] = 0;
        core1[i] = 0;
    }
    for(int i=0; i<4; i++){
        core[i] = 0;
    }

    int Ts = 3, Tc = 1, Tr = 1;
    int Ti_cloud = Ts + Tc + Tr;
    int E_l[n][3];
    int pk[] = {1,2,4};
    float ps = 0.5;
    float E_c = ps * Ts;
    for(int i=0; i<n; i++){
      for(int j=0; j<3; j++){
          E_l[i][j] = pk[j] * time_task_core_array[i][j];
      }
    }
    auto start = chrono::high_resolution_clock::now();
    ios_base::sync_with_stdio(false);

    //--------------------------------------------------------------
    //-----------Initial Scheduling --------------------------------
    //--------------------------------------------------------------

    //Initial Scheduling - Phase one
    primary_assignment(time_task_core_array,cloud,Ti_cloud,n,k);

    //Initial Scheduling - Phase two
    task_prioritzing(time_task_core_array,priority,priority_sorted,edge_matrix,computation_cost,cloud,Ti_cloud,n,k);

    //Initial Scheduling - Phase three
    execution_unit_selection(time_task_core_array,priority_sorted,edge_matrix,cloud,RT_l,RT_c,RT_ws,FT_ws,FT_wr,FT_l,finish_time,core,core1,n,k,Ts,Tr, Tc,scheduled_array);

    for(int i=0; i<4; i++){
        for(int j=0; j<n; j++){
            if(core1[j] == i){
                scheduled_array[i].push_back(j);
            }
        }
    }
    float E_total = 0;
    for(int i=0; i<n; i++){
        if(core1[i] == 3){
            E_total += E_c;
        }
        else{
            E_total += E_l[i][core1[i]];
        }
    }
    int start_time[n];
    for(int i=0; i<n; i++){
    	start_time[i] = RT_l[i];
    }

    int tmin = finish_time[n-1];
    int tmax = 1.2*tmin; //set the maximum limit as 1.2 x tmin
    cout<<"------Task scheduling result by the initial scheduling algorithm------"<<endl;
    for(int i=0; i<scheduled_array.size(); i++){
        if(i == 3){
            cout<<"Cloud: "<<endl;
        }
        else{
            cout<<"Core"<<i+1<<": "<<endl;
        }
        for(int j=0; j<scheduled_array[i].size(); j++){
        	cout<<"      |--Task - "<<scheduled_array[i][j]+1<<" [start time: "<<start_time[scheduled_array[i][j]]<<" finish time: "<<finish_time[scheduled_array[i][j]]<<" ]"<<endl;
        }
        cout<<endl;
    }
    cout<<"Initial Energy Consumption: "<<E_total<<"   Initial Completion Time: "<<tmin<<endl;
    cout<<endl;

    auto initial_end = chrono::high_resolution_clock::now();
	double time_taken_1 = chrono::duration_cast<chrono::nanoseconds>(initial_end - start).count();
	time_taken_1 *= 1e-9;
	cout << endl << "Running time (for initial scheduling) is : " << fixed << time_taken_1 << setprecision(9);
	cout << " sec" << endl<<endl<<endl;


	//--------------------------------------------------------------
	//-----------Task Migration --------------------------------
	//--------------------------------------------------------------


	//Task Migration - outer loop + Kernel algorithm
    task_migration(scheduled_array,time_task_core_array,edge_matrix,core1,tmax,tmin,E_total,n,k,start_time,finish_time,E_c,E_l);

    auto end = chrono::high_resolution_clock::now();
    double time_taken_2 = chrono::duration_cast<chrono::nanoseconds>(end - start).count();
    time_taken_2 *= 1e-9;
    cout << endl << "Running time (for MCC task scheduling) is : " << fixed << time_taken_2 << setprecision(9);
    cout << " sec" << endl;
    return 0;
}

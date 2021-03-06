#include <iostream>
#include <cstdlib>
#include <climits>
#include <vector>
#include <fstream>
#include <algorithm>
#include <cmath>
#include <iomanip>

using namespace std;

double SBpdf(double x, double xmin, double b, double l) {
	 return 1/((1+b)*(exp(-b/l*xmin)-exp(-b))+b*(exp(-b)-xmin/l*exp(-b/l*xmin)))*(2*b/l+b*b/(l*l)*(l-x))*exp(-b/l*x);
}

double SBcdf(double x, double xmin, double b, double l) {
	return ( (1+b)*(exp(-b/l*x)-exp(-b)) + b*(exp(-b)-x/l*exp(-b/l*x)) )/( (1+b)*(exp(-b/l*xmin)-exp(-b)) + b*(exp(-b)-xmin/l*exp(-b/l*xmin)) );
}

double SBlikelihood(vector<long> vec, long xmin, double b, double l) {
	long n = vec.size();
	double sum = 0.0;
	for(long i=0;i<n;i++) {
		sum += log(2+b*(l-vec[i])) - b*vec[i];
	}
	sum = -log( exp(-b*xmin)*(1+b*l-b*xmin) - exp(-b*l) ) + log(b) + sum/n; //-log(exp(bsul*(l-xmin))-1))
	return sum;
}

double DbSBlikelihood(vector<long> vec, long xmin, double b, double l) {
	long n = vec.size();
	double sum = 0.0;
	for(long i=0;i<n;i++) {
		sum += -vec[i]+2/b*(1+1/(b*(l-xmin)-2));
	}
	sum = l - (2+b*(l-xmin))*(l-xmin)/((1+b*(l-xmin))-exp(b*(xmin-l))) + sum/n;
	//cout << "Db: " << sum << endl;
	return sum;
}

double DlSBlikelihood(vector<long> vec, long xmin, double b, double l) {
	long n = vec.size();
	double sum = 0.0;
	for(long i=0;i<n;i++) {
		sum += b*( exp(b*(xmin-l)) * (3+b*(l-vec[i])) + (1+b*(xmin-vec[i])) )/( (1+b*(l-xmin)+exp(b*(xmin-l)))*(-2+b*(vec[i]-l)) );
	}
	//cout << "Dl: " << sum << endl;
	return sum;
}

double calc_D(vector<long> vec, double b, double l, long xmin, long xmax) {
	double d = 0.0;
	double dn = 0.0;
	double c = 0.0;
	long num = vec.size();
	for(long j=xmin+(xmax-xmin)/100;j<xmax;j+=(xmax-xmin)/100) {
		c = 0.0;
		for(long i=0;i<vec.size();i++) {
			if(vec[i]>=j) c++;
		}
		c = c/num;
		//cout << j << "\t" << c/num << "\t" << pow(j/xmin,1-alpha) << "\t" << abs(c/num-pow(j/xmin,1-alpha)) << endl;
		dn = abs(c-SBcdf(static_cast<double>(j),xmin,b,l));
		if(dn>d) d=dn;
		//cout << c << " , " << SBcdf(static_cast<double>(j),xmin,b,l) << endl;
	}
	return d;	
}


vector<double> minimizing_l(vector<long> vec, long xmin, double maxb, double maxl) {
	double bstep = maxb/50;
	double lstep = maxl/50;
	double bstart = bstep;
	double lstart = lstep;
	//cout << "bstep: " << bstep << "\tlstep: " << lstep << endl;
	double likelihood = 0.0;
	double maxlikelihood = SBlikelihood(vec, xmin, bstart, lstart);
	//cout << "min start likeli: " << minlikelihood << endl;
	for(double b=bstep;b<=maxb;b+=bstep) {
		for(double l=lstep;l<=maxl;l+=lstep) {
			likelihood = SBlikelihood(vec, xmin, b, l);
			//cout << likelihood << " < " << minlikelihood << endl;
			if(likelihood>maxlikelihood) {
				maxlikelihood = likelihood;
				bstart = b;
				lstart = l;
			}
		}
	}
	long cUpper = *max_element(vec.begin(), vec.end());
	//cout << "1 D: " << setw(10) << calc_D(vec, bstart*lstart, lstart, xmin, cUpper) << "\tmax l: " << setw(10) << maxlikelihood  << "\tlstart: " << setw(10) << lstart  << "\tbstart: " << setw(10) << bstart << endl;
	double bt = bstart;
	double lt = lstart;
	if(bt<bstep/10) bt += bstep/10;
	for(double b=bt-bstep;b<=bt+bstep;b+=bstep/10) {
		for(double l=lt-lstep;l<=lt+lstep;l+=lstep/5) {
			likelihood = SBlikelihood(vec, xmin, b, l);
			//cout << likelihood << " < " << minlikelihood << endl;
			if(likelihood>maxlikelihood) {
				maxlikelihood = likelihood;
				bstart = b;
				lstart = l;
			}
		}
	}
	//cout << "2 D: " << setw(10) << calc_D(vec, bstart*lstart, lstart, xmin, cUpper) << "\tmax l: " << setw(10) << maxlikelihood << "\tlstart: " << setw(10) << lstart  << "\tbstart: " << setw(10) << bstart << endl;
	//cout << "b0: " << bstart << " | l0: " << lstart << endl;
	double bold = 0.0;
	double bnew = bstart;
	double lold = 0.0;
	double lnew = lstart;
	double bprec = 1e-8;
	double lprec = 1e-8;
	double eps = 1e-9;
	double j = 0.0;
	likelihood = maxlikelihood;
	while( abs(bnew-bold)>bprec && abs(lnew-lold)>lprec) {
		/*if(SBlikelihood(vec, xmin, bnew, lnew)<likelihood) {
			bnew = bold;
			lnew = lold;	
		} else {*/
			bold = bnew;
			lold = lnew;
			/*likelihood=SBlikelihood(vec, xmin, bold, lold);
		}*/
		bnew = bold + eps * exp(-j) * DbSBlikelihood(vec, xmin, bold, lold);
		lnew = lold + eps * exp(-j) * DlSBlikelihood(vec, xmin, bold, lold);
		if(bnew<=0.0) bnew=bstart/100;
		//cout << "bnew: " << bnew << "\tbold: " << lnew << endl;
		j+=0.00001;
	}
	//cout << "f D: " << setw(10) << calc_D(vec, bnew*lnew, lnew, xmin, cUpper) << "\tmax l: " << setw(10) << SBlikelihood(vec, xmin, bnew, lnew)  << "\tl: " << setw(10) << lnew  << "\tbnew: " << setw(10) << bnew << endl;
	vector<double> par;
	par.push_back(bnew);
	par.push_back(lnew);
	return par;
}


int main(int argc, char *argv[]) {
	srand(time(0));
	string datafilename = argv[1];
	long cut = atol(argv[2]);
	vector<long> distanze;
	long temp = 0;
	ifstream datafile(datafilename.c_str());
	while(!datafile.eof()){
		datafile >> temp;
		if(temp<cut) {
			distanze.push_back(temp);
		}
	}
	datafile.close();
	sort(distanze.begin(),distanze.end());
	reverse(distanze.begin(),distanze.end());
	long numtotdistanze = distanze.size();
	long totallenght = 0;
	long maxdistance = 0;
	for(long i=0;i<numtotdistanze;i++) {
		totallenght += distanze[i];
		if(distanze[i]>maxdistance)
			maxdistance=distanze[i];
		//cout << distanze[i] << " ";
	}
	cout << "Number of segments: " << numtotdistanze << endl;
	cout << "Longest segment: " << maxdistance << endl;
	cout << "Total lenght: " << totallenght << endl;

	double* xmin = new double[maxdistance/2];
	double* l = new double[maxdistance/2];
	double* b = new double[maxdistance/2];
	double* D = new double[maxdistance/2];
	vector<long> vec;
	vector<double> res;
	ofstream outfile("out.dat");
	int resolution = 2000;
	for(long i=1;i<maxdistance/2;i+=maxdistance/resolution) {
		vec.clear();
		for(long j=0;j<numtotdistanze;j++) {
			//cout << j << "\t" << distanze[j] << endl;
			if(distanze[j]>=i) vec.push_back(distanze[j]);
		}
		res = minimizing_l(vec, i, static_cast<double>(numtotdistanze)/totallenght, static_cast<double>(totallenght));
		b[i] = res[0];
		l[i] = res[1];
		D[i] = calc_D(vec, b[i]*l[i], l[i], i, maxdistance);
		//cout << i << "\t" << alpha[i] << "\t" << D[i] << endl;
		//cout << i << "\t" << b[i] << "\t" << l[i] << "\t" << D[i] << endl;
		outfile << i << "\t" << b[i] << "\t" << l[i] << "\t" << D[i] << endl;
	}
	outfile.close();

	double bestxmin = 0.0;
	double bestl = 0.0;
	double bestb = 0.0;
	double bestd = 1.0;
	for(long i=1;i<maxdistance/2;i+=maxdistance/resolution) {
		if(D[i]<bestd) {
			bestxmin = i;
			bestl = l[i];
			bestb = b[i];
			bestd = D[i];
		}
	}
	cout << "Best b/l: " << bestb << endl << "Best l: " << bestl << endl << "Distance: " << bestd << endl;

	double c = 0.0;
	for(long j=0;j<numtotdistanze;j++) {
		if(distanze[j]>=bestxmin) c++;
	}
	c /= numtotdistanze;
	cout << "SB fraction: " << c/SBcdf(bestxmin, 0, bestb*bestl, bestl) << endl;
	
	/*long k = 0;
	double d = 2.0;
	for(long i=1;i<maxdistance/2;i+=maxdistance/1000) {
		if(abs(D[i])<d) {
			d=abs(D[i]);
			k=i;
		}
	}
	vec.clear();
	vector<long> vecless;
	for(long j=0;j<numtotdistanze;j++) {
		//cout << j << "\t" << distanze[j] << endl;
		if(distanze[j]>k) vec.push_back(distanze[j]);
		else vecless.push_back(distanze[j]);
	}*/
	/*double Z = calc_Z(D[k], alpha[k], k, maxdistance,vec.size(),vecless);
	cout << "xmin: " << k << endl << "alpha: " << alpha[k] << endl << "distance: " << D[k] << endl << "Z-score: " << Z << endl;*/

	delete [] xmin;
	delete [] b;
	delete [] l;
	delete [] D;
	cout << '\a';
	return 0;
}

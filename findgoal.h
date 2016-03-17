struct analyze_t{
	Mat * src;
	float * offAngle;
	float * distance;
	bool * foundGoal;
	bool debug;
};

void findGoal(int blob_size, void * data);

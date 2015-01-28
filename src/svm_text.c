#include <pebble.h>
#include <math.h>

// floating point
static int nr_class = 5;        
static int nr_feature = 13;
static int label[5] = {0, 2, 4, 3, 1};
// static double bias = 1;

double w[14][5] = 
{{-1.03275047031219, 0.005224473218930199, 2.958942574352583, -0.000606145054034933, -0.0001624289816346667}, 
{0.02145692171197216, 0.1064379433092949, -0.9320323013775328, -0.0001656486177286026, -0.0005566600009759561},
{0.2403446119165691, 0.005397008513559169, -0.1359741936248635, -5.9914778309314e-05, 0.0001238255030626353},
{0.2684143123516114, 0.8810555866360393, -0.02802732509440842, -0.001323917684819893, 0.01648736620003382},
{-1.490703533338165, 0.3003527409727323, -0.2548201218917274, 0.0001504137000960774, -0.01215680056798108},
{-0.9107950112718237, -0.1839298299282369, 0.3836455100818045, 0.001061270481514338, -0.02905403228141445},
{0.05975087192420474, -0.02156084821797723, 0.3194197417365757, -0.0001707486079104581, 0.0005246157799189107}, 
{0.05358840121027502, 0.00146383043171352, -0.2483196019463109, 0.000266562814305505, -0.001190907579717839},
{0.0004403167001597536, 0.03965273874265814, -1.466572511668493, -0.0002402113885083059, 0.0002496167513172413}, 
{-2.0401095945234, 1.721289106392673, -0.4759015041481059, 0.006085843705721446, 0.002059516155014522},
{2.182690279973874, -1.087502753916549, 0.1447388805487913, 0.007345435017321821, -0.02689169354493303},
{1.098937619797034, 0.1144414212614168, -2.183818588704399, -0.02630890528909002, 0.004709132216797801},
{0.1018919820564867, 0.05058179571703999, -0.4187636678446133, -0.0001144483708370676, -0.0006501125323499776}, 
{-1.210214534850028, 0.624394294203917, -4.510092962796866, -1.00906264093116, -1.044212923599961}};


static int true_label[1] = {0};
static double samples[2][14] = {{0.0666667, -0.666667, 0, -1, -1, -1, 0.561691, 1, -0.199082, -0.861474, -0.912613, -0.801965, -1, 1},
{0.666667, 1, -0.454545, -1, -1, -1, 0.237468, -0.787062, 0.0833118, -0.99107, -0.996555, -1, -0.821982, 1 }};

static double dec_values[5];

double predict(int index)
{
    int idx, i;
    int n = nr_feature + 1;
    int nr_w = nr_class;
    
    for(i = 0;i < nr_w; i++)
        dec_values[i] = 0;


    for (idx = 1; idx <= n; idx++) {
        if (samples[index][idx - 1] != 0.0) {
            for(i=0;i<nr_w;i++) {
                dec_values[i] += w[idx - 1][i] * samples[index][idx - 1];
                //printf("idx=%d i=%d wvalue=%lf xvalue=%lf\n", idx, i, w[idx - 1][i], samples[index][idx - 1]);
            }
        }

    }

    int dec_max_idx = 0;
    for(i = 1; i < nr_class; i++) {
        //printf("dec[%d]=%lf\n", i, dec_values[i]);
        if(dec_values[i] > dec_values[dec_max_idx])
            dec_max_idx = i;
    }
    //printf("predict index=%d label=%d\n", dec_max_idx, label[dec_max_idx]);
    return label[dec_max_idx];
}


int do_predict()
{
    int correct = 0;
    for (int i = 0; i < 200; i++) {
      int predict_label = predict(0);
      //printf("%d\n", predict_label);
  
      if(predict_label == true_label[0]) {
          ++correct;
      }
    }

    return correct;
}

Window *window;
TextLayer *text_layer;

void handle_init(void) {
	// Create a window and text layer
	window = window_create();
	text_layer = text_layer_create(GRect(0, 0, 144, 154));
  
  
  time_t sec1, sec2;
  uint16_t ms1, ms2;
  int32_t t1, t2, dt;
  
  time_ms(&sec1, &ms1);
  
  // the actual classification
  int correct = do_predict();
  
  time_ms(&sec2, &ms2);
  
  t1 = (int32_t)1000*(int32_t)sec1 + (int32_t)ms1;
  t2 = (int32_t)1000*(int32_t)sec2 + (int32_t)ms2;
  
  // dt is the time spent in milliseconds
  dt = t2 - t1;
  
  char str[20];
  snprintf(str, 20, "%d,%d", correct, (int)dt);
  
	// Set the text, font, and text alignment
	text_layer_set_text(text_layer, str);
	text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
	text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
	
	// Add the text layer to the window
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(text_layer));

	// Push the window
	window_stack_push(window, true);
	
	// App Logging!
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Just pushed a window!");
}

void handle_deinit(void) {
	// Destroy the text layer
	text_layer_destroy(text_layer);
	
	// Destroy the window
	window_destroy(window);
}

int main(void) {
	handle_init();
	app_event_loop();
	handle_deinit();
}
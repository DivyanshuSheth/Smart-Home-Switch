! pip -q install micromlgen

from micromlgen import port
from sklearn.svm import SVR
import pandas as pd
from sklearn import preprocessing
from sklearn.preprocessing import OneHotEncoder
from sklearn.model_selection import train_test_split
from sklearn.pipeline import make_pipeline
from sklearn.linear_model import LinearRegression

data = pd.read_csv('/content/microml_dataset.csv')
X = data[['day', 'month', 'holiday']]
enc = OneHotEncoder(handle_unknown='ignore')
features = enc.fit_transform(X)
X_train, X_test, y_train, y_test = train_test_split(features, data['arrival_time'], test_size = 0.25, shuffle = True, random_state = 42)
regr = LinearRegression()
regr.fit(X_train, y_train)
print(port(regr))

"""
Output: (Correspondng C code):

#pragma once
#include <cstdarg>
namespace Eloquent {
    namespace ML {
        namespace Port {
            class LinearRegression {
                public:
                    /**
                    * Predict class for features vector
                    */
                    float predict(float *x) {
                        return dot(x, -7.490451107479, -2.131955033807, -14.864346220156, 17.857421237046, -26.076434085877, 10.428375372749, 22.277389837524, 20.985167964303, 13.878946252880, 4.158854226772, 4.060741899768, -35.252109602183, -18.319326021150, -7.546801716586, 30.012158885118, -41.273935747967, 24.133536036778, -8.490986034305, 13.653753856572, -1.069948671851, 1.069948671850) + 2027.519167910283;
                    }

                protected:
                    /**
                    * Compute dot product
                    */
                    float dot(float *x, ...) {
                        va_list w;
                        va_start(w, 21);
                        float dot = 0.0;

                        for (uint16_t i = 0; i < 21; i++) {
                            const float wi = va_arg(w, double);
                            dot += x[i] * wi;
                        }

                        return dot;
                    }
                };
            }
        }
"""
